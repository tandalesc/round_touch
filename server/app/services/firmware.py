"""Firmware-related helpers."""

from pathlib import Path

from app.database import Database, compute_firmware_hmac


async def get_firmware_info(db: Database, board: str) -> dict | None:
    """Get firmware metadata for a board."""
    return await db.get_latest_firmware(board)


def get_firmware_path(storage_dir: str, filename: str) -> Path:
    """Resolve a firmware file path within storage."""
    return Path(storage_dir) / "firmware" / filename
