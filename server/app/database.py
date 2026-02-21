"""SQLite database setup, schema creation, and auto-import migration."""

import hashlib
import hmac as hmac_mod
import json
import logging
from datetime import datetime, timezone
from pathlib import Path

import aiosqlite

logger = logging.getLogger(__name__)

SCHEMA_SQL = """
CREATE TABLE IF NOT EXISTS boards (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS manifests (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    board_id INTEGER NOT NULL REFERENCES boards(id),
    manifest JSON NOT NULL,
    version INTEGER NOT NULL DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE
);

CREATE UNIQUE INDEX IF NOT EXISTS idx_manifests_active
    ON manifests(board_id) WHERE is_active = TRUE;

CREATE TABLE IF NOT EXISTS firmware (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    board_id INTEGER NOT NULL REFERENCES boards(id),
    version TEXT NOT NULL,
    filename TEXT NOT NULL,
    size INTEGER NOT NULL,
    hmac TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_firmware_board ON firmware(board_id);

CREATE TABLE IF NOT EXISTS dynamic_content (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    content_key TEXT UNIQUE NOT NULL,
    rendered_text TEXT NOT NULL,
    etag TEXT NOT NULL,
    expires_at TIMESTAMP NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS dynamic_configs (
    content_key TEXT PRIMARY KEY,
    component_type TEXT NOT NULL,
    template TEXT NOT NULL,
    entities TEXT NOT NULL DEFAULT '',
    model TEXT DEFAULT '',
    ttl_seconds INTEGER DEFAULT 300
);
"""


class Database:
    def __init__(self, db_path: str):
        self.db_path = db_path
        self.db: aiosqlite.Connection | None = None

    async def connect(self):
        self.db = await aiosqlite.connect(self.db_path)
        self.db.row_factory = aiosqlite.Row
        await self.db.execute("PRAGMA journal_mode=WAL")
        await self.db.execute("PRAGMA foreign_keys=ON")
        await self.db.executescript(SCHEMA_SQL)
        await self.db.commit()

    async def close(self):
        if self.db:
            await self.db.close()

    async def is_empty(self) -> bool:
        async with self.db.execute("SELECT COUNT(*) FROM boards") as cur:
            row = await cur.fetchone()
            return row[0] == 0

    # -- Board operations --

    async def get_board_id(self, name: str) -> int | None:
        async with self.db.execute(
            "SELECT id FROM boards WHERE name = ?", (name,)
        ) as cur:
            row = await cur.fetchone()
            return row[0] if row else None

    async def create_board(self, name: str) -> int:
        async with self.db.execute(
            "INSERT INTO boards (name) VALUES (?)", (name,)
        ) as cur:
            await self.db.commit()
            return cur.lastrowid

    async def list_boards(self) -> list[dict]:
        async with self.db.execute(
            """SELECT b.name,
                      (SELECT COUNT(*) FROM json_each(
                          (SELECT json_extract(m.manifest, '$.screens')
                           FROM manifests m
                           WHERE m.board_id = b.id AND m.is_active = TRUE)
                      )) as screen_count
               FROM boards b
               ORDER BY b.name"""
        ) as cur:
            rows = await cur.fetchall()
            return [{"name": r[0], "screenCount": r[1] or 0} for r in rows]

    # -- Manifest operations --

    async def get_active_manifest(self, board_name: str) -> dict | None:
        async with self.db.execute(
            """SELECT m.manifest FROM manifests m
               JOIN boards b ON b.id = m.board_id
               WHERE b.name = ? AND m.is_active = TRUE""",
            (board_name,),
        ) as cur:
            row = await cur.fetchone()
            return json.loads(row[0]) if row else None

    async def save_manifest(self, board_name: str, manifest: dict):
        board_id = await self.get_board_id(board_name)
        if board_id is None:
            raise ValueError(f"Board not found: {board_name}")

        # Extract dynamic configs first — this injects content_key into the
        # manifest dict so the device can read it without computing hashes.
        await self._extract_dynamic_configs(manifest)

        # Deactivate previous active manifest
        await self.db.execute(
            "UPDATE manifests SET is_active = FALSE WHERE board_id = ? AND is_active = TRUE",
            (board_id,),
        )
        # Insert new active manifest (now includes injected content_key fields)
        version = manifest.get("version", 1)
        await self.db.execute(
            "INSERT INTO manifests (board_id, manifest, version) VALUES (?, ?, ?)",
            (board_id, json.dumps(manifest, ensure_ascii=False), version),
        )
        await self.db.commit()

    async def _extract_dynamic_configs(self, manifest: dict):
        """Scan manifest for DynamicText/LLMText nodes and store their configs."""
        configs = []
        for screen in manifest.get("screens", {}).values():
            self._walk_nodes(screen, configs)

        for cfg in configs:
            await self.db.execute(
                """INSERT OR REPLACE INTO dynamic_configs
                   (content_key, component_type, template, entities, model, ttl_seconds)
                   VALUES (?, ?, ?, ?, ?, ?)""",
                (cfg["content_key"], cfg["component_type"], cfg["template"],
                 cfg["entities"], cfg["model"], cfg["ttl_seconds"]),
            )
        await self.db.commit()

    def _walk_nodes(self, node: dict, configs: list):
        """Walk a component tree, extracting dynamic configs and injecting content_key."""
        if not isinstance(node, dict):
            return
        ntype = node.get("type", "")
        if ntype == "DynamicText":
            template = node.get("template", "")
            entities = node.get("entities", "")
            ttl = node.get("props", {}).get("ttl", 60)
            key = compute_content_key(template, entities, "")
            node["content_key"] = key  # inject so device can read it
            configs.append({
                "content_key": key,
                "component_type": "DynamicText",
                "template": template,
                "entities": entities,
                "model": "",
                "ttl_seconds": ttl,
            })
        elif ntype == "LLMText":
            prompt = node.get("prompt", "")
            model = node.get("model", "qwen3-8b")
            entities = node.get("entities", "")
            ttl = node.get("props", {}).get("ttl", 300)
            key = compute_content_key(prompt, entities, model)
            node["content_key"] = key  # inject so device can read it
            configs.append({
                "content_key": key,
                "component_type": "LLMText",
                "template": prompt,
                "entities": entities,
                "model": model,
                "ttl_seconds": ttl,
            })
        for child in node.get("children", []):
            self._walk_nodes(child, configs)

    # -- Firmware operations --

    async def get_latest_firmware(self, board_name: str) -> dict | None:
        async with self.db.execute(
            """SELECT f.version, f.filename, f.size, f.hmac
               FROM firmware f
               JOIN boards b ON b.id = f.board_id
               WHERE b.name = ?
               ORDER BY f.created_at DESC LIMIT 1""",
            (board_name,),
        ) as cur:
            row = await cur.fetchone()
            if not row:
                return None
            return {
                "version": row[0],
                "filename": row[1],
                "size": row[2],
                "hmac": row[3],
            }

    async def add_firmware(self, board_name: str, version: str,
                           filename: str, size: int, hmac_hex: str):
        board_id = await self.get_board_id(board_name)
        if board_id is None:
            raise ValueError(f"Board not found: {board_name}")
        await self.db.execute(
            """INSERT INTO firmware (board_id, version, filename, size, hmac)
               VALUES (?, ?, ?, ?, ?)""",
            (board_id, version, filename, size, hmac_hex),
        )
        await self.db.commit()

    # -- Dynamic content cache --

    async def get_dynamic_config(self, content_key: str) -> dict | None:
        async with self.db.execute(
            "SELECT * FROM dynamic_configs WHERE content_key = ?",
            (content_key,),
        ) as cur:
            row = await cur.fetchone()
            if not row:
                return None
            return {
                "content_key": row[0],
                "component_type": row[1],
                "template": row[2],
                "entities": row[3],
                "model": row[4],
                "ttl_seconds": row[5],
            }

    async def get_cached_content(self, content_key: str) -> dict | None:
        async with self.db.execute(
            "SELECT rendered_text, etag, expires_at FROM dynamic_content WHERE content_key = ?",
            (content_key,),
        ) as cur:
            row = await cur.fetchone()
            if not row:
                return None
            expires_at = datetime.fromisoformat(row[2])
            if expires_at.tzinfo is None:
                expires_at = expires_at.replace(tzinfo=timezone.utc)
            return {
                "text": row[0],
                "etag": row[1],
                "expired": datetime.now(timezone.utc) > expires_at,
            }

    async def invalidate_all_cached_content(self):
        """Expire all cached dynamic content so the next request triggers a fresh resolve."""
        await self.db.execute(
            "UPDATE dynamic_content SET expires_at = ?",
            (datetime.now(timezone.utc).isoformat(),),
        )
        await self.db.commit()

    async def set_cached_content(self, content_key: str, text: str,
                                 etag: str, ttl_seconds: int):
        expires_at = datetime.now(timezone.utc).isoformat()
        # Compute future expiry
        from datetime import timedelta
        expires = datetime.now(timezone.utc) + timedelta(seconds=ttl_seconds)
        await self.db.execute(
            """INSERT OR REPLACE INTO dynamic_content
               (content_key, rendered_text, etag, expires_at)
               VALUES (?, ?, ?, ?)""",
            (content_key, text, etag, expires.isoformat()),
        )
        await self.db.commit()


def compute_content_key(template: str, entities: str, model: str) -> str:
    """Compute a content key from component config — must match device-side computation."""
    entity_list = sorted(e.strip() for e in entities.split(",") if e.strip())
    canonical = json.dumps({
        "entities": ",".join(entity_list),
        "model": model,
        "template": template,
    }, sort_keys=True)
    return hashlib.sha256(canonical.encode()).hexdigest()[:16]


def compute_firmware_hmac(filepath: Path, secret_key: str) -> str:
    h = hmac_mod.new(secret_key.encode(), digestmod=hashlib.sha256)
    with open(filepath, "rb") as f:
        while chunk := f.read(8192):
            h.update(chunk)
    return h.hexdigest()


async def auto_import(db: Database, ui_dir: str, firmware_dir: str,
                      secret_key: str, firmware_version: str,
                      storage_dir: str):
    """Import existing JSON files and firmware into the database on first run."""
    if not await db.is_empty():
        logger.info("Database already populated, skipping import.")
        return

    logger.info("First run — importing existing data into SQLite...")

    # Import UI manifests
    ui_path = Path(ui_dir)
    if ui_path.exists():
        for board_dir in sorted(ui_path.iterdir()):
            screens_path = board_dir / "screens.json"
            if board_dir.is_dir() and screens_path.exists():
                board_name = board_dir.name
                await db.create_board(board_name)
                with open(screens_path) as f:
                    manifest = json.load(f)
                await db.save_manifest(board_name, manifest)
                logger.info(f"  Imported manifest: {board_name}")

    # Import firmware
    fw_path = Path(firmware_dir)
    if fw_path.exists():
        storage = Path(storage_dir) / "firmware"
        storage.mkdir(parents=True, exist_ok=True)
        for board_dir in sorted(fw_path.iterdir()):
            fw_file = board_dir / "firmware.bin"
            if board_dir.is_dir() and fw_file.exists():
                board_name = board_dir.name
                # Ensure board exists
                if await db.get_board_id(board_name) is None:
                    await db.create_board(board_name)
                # Copy firmware to storage
                dest_dir = storage / board_name
                dest_dir.mkdir(parents=True, exist_ok=True)
                dest = dest_dir / "firmware.bin"
                import shutil
                shutil.copy2(fw_file, dest)
                # Compute HMAC and store metadata
                hmac_hex = compute_firmware_hmac(dest, secret_key)
                size = dest.stat().st_size
                rel_path = f"{board_name}/firmware.bin"
                await db.add_firmware(board_name, firmware_version,
                                      rel_path, size, hmac_hex)
                logger.info(f"  Imported firmware: {board_name} ({size} bytes)")

    logger.info("Import complete.")
