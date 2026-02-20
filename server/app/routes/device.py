"""Device API endpoints — firmware OTA and UI manifest serving."""

from pathlib import Path

from fastapi import APIRouter, HTTPException, Query
from fastapi.responses import JSONResponse, StreamingResponse

from app.database import Database
from app.services.firmware import get_firmware_path

router = APIRouter(prefix="/api", tags=["device"])


def create_router(db: Database, storage_dir: str) -> APIRouter:
    @router.get("/version")
    async def version(board: str = Query(..., description="Board identifier")):
        fw = await db.get_latest_firmware(board)
        if not fw:
            raise HTTPException(404, f"No firmware for board: {board}")
        return {
            "version": fw["version"],
            "size": fw["size"],
            "hmac": fw["hmac"],
        }

    @router.get("/firmware")
    async def firmware(board: str = Query(..., description="Board identifier")):
        fw = await db.get_latest_firmware(board)
        if not fw:
            raise HTTPException(404, f"No firmware for board: {board}")

        fw_path = get_firmware_path(storage_dir, fw["filename"])
        if not fw_path.exists():
            raise HTTPException(404, f"Firmware file missing: {fw['filename']}")

        def stream():
            with open(fw_path, "rb") as f:
                while chunk := f.read(8192):
                    yield chunk

        return StreamingResponse(
            stream(),
            media_type="application/octet-stream",
            headers={
                "Content-Length": str(fw["size"]),
                "Content-Disposition": "attachment; filename=firmware.bin",
            },
        )

    @router.get("/ui/screens")
    async def ui_screens(board: str = Query(..., description="Board identifier")):
        manifest = await db.get_active_manifest(board)
        if not manifest:
            raise HTTPException(404, f"No UI manifest for board: {board}")
        return JSONResponse(content=manifest)

    return router
