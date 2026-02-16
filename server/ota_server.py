"""
OTA Update Server for round_touch firmware.

Serves firmware binaries, version metadata, UI screen manifests,
and a web-based manifest editor.
Uses HMAC-SHA256 with a pre-shared key so devices can verify binary authenticity.

Usage:
    python ota_server.py --config server.toml

The server reads firmware binaries from the PlatformIO build output directory
and UI manifests from the ui directory.
Expected layout:
    firmware_dir/
      makerfabs_round_128/
        firmware.bin
      waveshare_s3_lcd_7/
        firmware.bin
    ui_dir/
      makerfabs_round_128/
        screens.json
      simulator/
        screens.json
"""

import argparse
import hashlib
import hmac
import json
import shutil
import sys
import tomllib
from pathlib import Path

from fastapi import FastAPI, HTTPException, Query, Request
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse, StreamingResponse
from fastapi.staticfiles import StaticFiles

app = FastAPI(title="round_touch OTA Server")

# Allow Vite dev server (localhost:5173) during development
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:5173"],
    allow_methods=["*"],
    allow_headers=["*"],
)

CONFIG = {
    "firmware_dir": "",
    "secret_key": "",
    "version": "",
    "port": 8080,
    "ui_dir": "ui",
}

# ---------------------------------------------------------------------------
# Component schema — mirrors src/ui/registry/ComponentFactories.h
# Served to the web editor so it can dynamically build prop forms.
# ---------------------------------------------------------------------------
COMPONENT_SCHEMA = {
    "Text": {
        "container": False,
        "fields": {
            "text": {"type": "string", "default": "", "label": "Text content", "location": "top"},
        },
        "props": {
            "size": {"type": "int", "default": 3, "min": 1, "max": 5, "label": "Font size"},
            "color": {"type": "color", "default": None, "label": "Text color"},
        },
    },
    "Card": {
        "container": True,
        "fields": {},
        "props": {
            "bg": {"type": "color", "default": None, "label": "Background"},
            "border": {"type": "color", "default": None, "label": "Border"},
            "radius": {"type": "int", "default": None, "label": "Radius"},
            "pad": {"type": "int", "default": None, "label": "Padding"},
            "gap": {"type": "int", "default": None, "label": "Gap"},
        },
    },
    "FillScreen": {
        "container": True,
        "fields": {},
        "props": {
            "color": {"type": "color", "default": None, "label": "Background"},
            "pad": {"type": "int", "default": None, "label": "Padding"},
            "gap": {"type": "int", "default": None, "label": "Gap"},
        },
    },
    "FlexLayout": {
        "container": True,
        "fields": {},
        "props": {
            "direction": {"type": "enum", "options": ["row", "column"], "default": "column", "label": "Direction"},
            "gap": {"type": "int", "default": None, "label": "Gap"},
            "align": {"type": "enum", "options": ["left", "center", "right"], "default": "left", "label": "Align"},
        },
    },
    "ScrollContainer": {
        "container": True,
        "fields": {},
        "props": {
            "pad": {"type": "int", "default": None, "label": "Padding"},
            "gap": {"type": "int", "default": None, "label": "Gap"},
            "maxWidth": {"type": "int", "default": None, "label": "Max width"},
        },
    },
    "TitledCard": {
        "container": True,
        "fields": {},
        "props": {
            "icon": {"type": "icon", "default": "", "label": "Icon"},
            "title": {"type": "string", "default": "", "label": "Title"},
            "bg": {"type": "color", "default": None, "label": "Background"},
            "border": {"type": "color", "default": None, "label": "Border"},
        },
    },
    "GaugeCard": {
        "container": False,
        "fields": {},
        "props": {
            "label": {"type": "string", "default": "", "label": "Label"},
            "value": {"type": "string", "default": "", "label": "Value"},
        },
    },
    "HAToggle": {
        "container": False,
        "props": {},
        "fields": {
            "entity": {"type": "string", "default": "", "label": "HA entity ID", "location": "top"},
        },
    },
    "HAWeather": {
        "container": False,
        "props": {},
        "fields": {
            "entity": {"type": "string", "default": "", "label": "HA entity ID", "location": "top"},
        },
    },
    "HABinarySensor": {
        "container": False,
        "props": {},
        "fields": {
            "entity": {"type": "string", "default": "", "label": "HA entity ID", "location": "top"},
            "label": {"type": "string", "default": "", "label": "Display label", "location": "top"},
        },
    },
}

KNOWN_COMPONENT_TYPES = set(COMPONENT_SCHEMA.keys())


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def get_firmware_path(board: str) -> Path:
    return Path(CONFIG["firmware_dir"]) / board / "firmware.bin"


def compute_hmac(filepath: Path, secret_key: str) -> str:
    h = hmac.new(secret_key.encode(), digestmod=hashlib.sha256)
    with open(filepath, "rb") as f:
        while chunk := f.read(8192):
            h.update(chunk)
    return h.hexdigest()


def validate_manifest(manifest: dict) -> list[str]:
    """Validate a manifest dict. Returns a list of error strings (empty = valid)."""
    errors = []
    if "version" not in manifest:
        errors.append("Missing 'version' key")
    if not isinstance(manifest.get("tabs"), list):
        errors.append("Missing or invalid 'tabs' (must be array)")
    if not isinstance(manifest.get("screens"), dict):
        errors.append("Missing or invalid 'screens' (must be object)")
    if "default_screen" not in manifest:
        errors.append("Missing 'default_screen' key")

    if errors:
        return errors

    tab_ids = {t["id"] for t in manifest["tabs"] if isinstance(t, dict) and "id" in t}
    screen_ids = set(manifest["screens"].keys())

    for tid in tab_ids:
        if str(tid) not in screen_ids:
            errors.append(f"Tab id={tid} has no matching screen")

    if manifest["default_screen"] not in tab_ids:
        errors.append(f"default_screen={manifest['default_screen']} is not a valid tab id")

    def check_node(node, path="root"):
        if not isinstance(node, dict):
            errors.append(f"{path}: node is not an object")
            return
        ntype = node.get("type")
        if not ntype:
            errors.append(f"{path}: missing 'type'")
            return
        if ntype not in KNOWN_COMPONENT_TYPES:
            errors.append(f"{path}: unknown component type '{ntype}'")
        schema = COMPONENT_SCHEMA.get(ntype, {})
        if not schema.get("container") and node.get("children"):
            errors.append(f"{path}: leaf component '{ntype}' cannot have children")
        for i, child in enumerate(node.get("children", [])):
            check_node(child, f"{path}.children[{i}]")

    for sid, screen in manifest["screens"].items():
        check_node(screen, f"screens[{sid}]")

    return errors


# ---------------------------------------------------------------------------
# Device API endpoints (unchanged)
# ---------------------------------------------------------------------------

@app.get("/api/version")
async def version(board: str = Query(..., description="Board identifier")):
    firmware_path = get_firmware_path(board)
    if not firmware_path.exists():
        raise HTTPException(404, f"No firmware for board: {board}")

    signature = compute_hmac(firmware_path, CONFIG["secret_key"])
    size = firmware_path.stat().st_size

    return {
        "version": CONFIG["version"],
        "size": size,
        "hmac": signature,
    }


@app.get("/api/firmware")
async def firmware(board: str = Query(..., description="Board identifier")):
    firmware_path = get_firmware_path(board)
    if not firmware_path.exists():
        raise HTTPException(404, f"No firmware for board: {board}")

    def stream():
        with open(firmware_path, "rb") as f:
            while chunk := f.read(8192):
                yield chunk

    size = firmware_path.stat().st_size
    return StreamingResponse(
        stream(),
        media_type="application/octet-stream",
        headers={
            "Content-Length": str(size),
            "Content-Disposition": "attachment; filename=firmware.bin",
        },
    )


@app.get("/api/ui/screens")
async def ui_screens(board: str = Query(..., description="Board identifier")):
    ui_dir = Path(CONFIG["ui_dir"])
    screens_path = ui_dir / board / "screens.json"
    if not screens_path.exists():
        raise HTTPException(404, f"No UI manifest for board: {board}")

    with open(screens_path) as f:
        manifest = json.load(f)

    return JSONResponse(content=manifest)


# ---------------------------------------------------------------------------
# Editor API endpoints
# ---------------------------------------------------------------------------

@app.get("/api/editor/boards")
async def editor_boards():
    """List boards that have UI manifests."""
    ui_dir = Path(CONFIG["ui_dir"])
    boards = []
    if ui_dir.exists():
        for d in sorted(ui_dir.iterdir()):
            screens_path = d / "screens.json"
            if d.is_dir() and screens_path.exists():
                with open(screens_path) as f:
                    manifest = json.load(f)
                boards.append({
                    "name": d.name,
                    "screenCount": len(manifest.get("screens", {})),
                })
    return {"boards": boards}


@app.get("/api/editor/manifest")
async def editor_get_manifest(board: str = Query(..., description="Board identifier")):
    """Return the full manifest JSON for a board."""
    ui_dir = Path(CONFIG["ui_dir"])
    screens_path = ui_dir / board / "screens.json"
    if not screens_path.exists():
        raise HTTPException(404, f"No UI manifest for board: {board}")

    with open(screens_path) as f:
        manifest = json.load(f)

    return JSONResponse(content=manifest)


@app.put("/api/editor/manifest")
async def editor_put_manifest(
    request: Request,
    board: str = Query(..., description="Board identifier"),
):
    """Validate and save a manifest for a board. Backs up the previous version."""
    ui_dir = Path(CONFIG["ui_dir"])
    board_dir = ui_dir / board
    screens_path = board_dir / "screens.json"

    if not board_dir.exists():
        raise HTTPException(404, f"No board directory: {board}")

    try:
        manifest = await request.json()
    except Exception:
        raise HTTPException(422, "Invalid JSON body")

    errors = validate_manifest(manifest)
    if errors:
        raise HTTPException(422, {"errors": errors})

    # Backup current file before overwriting
    if screens_path.exists():
        shutil.copy2(screens_path, screens_path.with_suffix(".json.bak"))

    with open(screens_path, "w") as f:
        json.dump(manifest, f, indent=2, ensure_ascii=False)
        f.write("\n")

    return {"ok": True}


@app.post("/api/editor/boards")
async def editor_create_board(request: Request):
    """Create a new board directory with a minimal default manifest."""
    body = await request.json()
    name = body.get("name", "").strip()
    if not name or "/" in name or "\\" in name or ".." in name:
        raise HTTPException(422, "Invalid board name")

    ui_dir = Path(CONFIG["ui_dir"])
    board_dir = ui_dir / name
    if board_dir.exists():
        raise HTTPException(409, f"Board '{name}' already exists")

    board_dir.mkdir(parents=True)
    default_manifest = {
        "version": 1,
        "default_screen": 32,
        "tabs": [
            {"id": 32, "icon": "\uF015", "label": "Home"},
        ],
        "screens": {
            "32": {
                "type": "FlexLayout",
                "props": {"direction": "column", "align": "center", "gap": 12},
                "children": [
                    {"type": "Text", "props": {"size": 4}, "text": "\uF015 Home"},
                ],
            },
        },
    }
    with open(board_dir / "screens.json", "w") as f:
        json.dump(default_manifest, f, indent=2, ensure_ascii=False)
        f.write("\n")

    return {"ok": True}


@app.get("/api/editor/schema")
async def editor_schema():
    """Return the component type schema for the web editor."""
    return JSONResponse(content=COMPONENT_SCHEMA)


# ---------------------------------------------------------------------------
# Static file serving for the built web editor SPA
# ---------------------------------------------------------------------------

web_dist = Path(__file__).parent / "web" / "dist"
if web_dist.exists():
    app.mount("/", StaticFiles(directory=web_dist, html=True), name="web")


# ---------------------------------------------------------------------------
# CLI entrypoint
# ---------------------------------------------------------------------------

def main():
    import uvicorn

    parser = argparse.ArgumentParser(description="round_touch OTA Update Server")
    parser.add_argument("--config", default="server.toml", help="TOML config file")
    parser.add_argument("--firmware-dir", help="Override firmware directory")
    parser.add_argument("--secret-key", help="Override HMAC secret key")
    parser.add_argument("--version", help="Override firmware version string")
    parser.add_argument("--port", type=int, help="Override listen port")
    args = parser.parse_args()

    # Load config from TOML
    config_path = Path(args.config)
    if config_path.exists():
        with open(config_path, "rb") as f:
            file_config = tomllib.load(f)
        CONFIG.update(file_config)
    else:
        print(f"Warning: config file '{config_path}' not found, using defaults")

    # CLI args override config file
    if args.firmware_dir:
        CONFIG["firmware_dir"] = args.firmware_dir
    if args.secret_key:
        CONFIG["secret_key"] = args.secret_key
    if args.version:
        CONFIG["version"] = args.version
    if args.port:
        CONFIG["port"] = args.port

    # Validate required config
    if not CONFIG["firmware_dir"]:
        print("Error: firmware_dir is required (set in config or --firmware-dir)")
        sys.exit(1)
    if not CONFIG["secret_key"]:
        print("Error: secret_key is required (set in config or --secret-key)")
        sys.exit(1)
    if not CONFIG["version"]:
        print("Error: version is required (set in config or --version)")
        sys.exit(1)

    firmware_dir = Path(CONFIG["firmware_dir"])
    if not firmware_dir.exists():
        print(f"Error: firmware directory '{firmware_dir}' does not exist")
        sys.exit(1)

    ui_dir = Path(CONFIG["ui_dir"])
    print(f"OTA Server v{CONFIG['version']}")
    print(f"  Firmware dir: {firmware_dir.resolve()}")
    print(f"  UI dir: {ui_dir.resolve()}")
    print(f"  Port: {CONFIG['port']}")

    # List available boards
    boards = [d.name for d in firmware_dir.iterdir() if d.is_dir() and (d / "firmware.bin").exists()]
    if boards:
        print(f"  Boards: {', '.join(boards)}")
    else:
        print("  Warning: no firmware.bin found in any subdirectory")

    uvicorn.run(app, host="0.0.0.0", port=CONFIG["port"])


if __name__ == "__main__":
    main()
