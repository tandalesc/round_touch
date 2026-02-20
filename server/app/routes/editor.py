"""Editor API endpoints — manifest CRUD and schema serving."""

import json

from fastapi import APIRouter, HTTPException, Query, Request
from fastapi.responses import JSONResponse

from app.database import Database
from app.schema import COMPONENT_SCHEMA
from app.services.manifest import validate_manifest

router = APIRouter(prefix="/api/editor", tags=["editor"])


def create_router(db: Database) -> APIRouter:
    @router.get("/boards")
    async def editor_boards():
        """List boards that have UI manifests."""
        boards = await db.list_boards()
        return {"boards": boards}

    @router.get("/manifest")
    async def editor_get_manifest(
        board: str = Query(..., description="Board identifier"),
    ):
        """Return the full manifest JSON for a board."""
        manifest = await db.get_active_manifest(board)
        if not manifest:
            raise HTTPException(404, f"No UI manifest for board: {board}")
        return JSONResponse(content=manifest)

    @router.put("/manifest")
    async def editor_put_manifest(
        request: Request,
        board: str = Query(..., description="Board identifier"),
    ):
        """Validate and save a manifest for a board."""
        board_id = await db.get_board_id(board)
        if board_id is None:
            raise HTTPException(404, f"No board: {board}")

        try:
            manifest = await request.json()
        except Exception:
            raise HTTPException(422, "Invalid JSON body")

        errors = validate_manifest(manifest)
        if errors:
            raise HTTPException(422, {"errors": errors})

        await db.save_manifest(board, manifest)
        return {"ok": True}

    @router.post("/boards")
    async def editor_create_board(request: Request):
        """Create a new board with a minimal default manifest."""
        body = await request.json()
        name = body.get("name", "").strip()
        if not name or "/" in name or "\\" in name or ".." in name:
            raise HTTPException(422, "Invalid board name")

        existing = await db.get_board_id(name)
        if existing is not None:
            raise HTTPException(409, f"Board '{name}' already exists")

        await db.create_board(name)

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
        await db.save_manifest(name, default_manifest)
        return {"ok": True}

    @router.get("/schema")
    async def editor_schema():
        """Return the component type schema for the web editor."""
        return JSONResponse(content=COMPONENT_SCHEMA)

    return router
