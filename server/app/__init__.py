"""FastAPI application factory."""

import logging
from contextlib import asynccontextmanager
from pathlib import Path

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles

from app.config import Config
from app.database import Database, auto_import
from app.routes.device import create_router as device_router
from app.routes.editor import create_router as editor_router
from app.routes.dynamic import create_router as dynamic_router
from app.routes.datasources import create_router as datasources_router
from app.services.datasources import create_registry
from app.services.llm import LLMService

logger = logging.getLogger(__name__)


def create_app(config: Config) -> FastAPI:
    db = Database(config.db_path)
    datasources = create_registry(config.homeassistant)
    llm = LLMService(config.llm)

    @asynccontextmanager
    async def lifespan(app: FastAPI):
        # Startup
        await db.connect()
        await auto_import(
            db,
            ui_dir=config.ui_dir,
            firmware_dir=config.firmware_dir,
            secret_key=config.secret_key,
            firmware_version=config.version,
            storage_dir=config.storage_dir,
        )
        yield
        # Shutdown
        await db.close()

    app = FastAPI(title="round_touch Server", lifespan=lifespan)

    # CORS for Vite dev server
    app.add_middleware(
        CORSMiddleware,
        allow_origins=["http://localhost:5173"],
        allow_methods=["*"],
        allow_headers=["*"],
    )

    # Register routes
    app.include_router(device_router(db, config.storage_dir))
    app.include_router(editor_router(db))
    app.include_router(dynamic_router(db, datasources, llm))
    app.include_router(datasources_router(datasources, config.homeassistant))

    # Serve built web editor SPA
    web_dist = Path(__file__).parent.parent / "web" / "dist"
    if web_dist.exists():
        app.mount("/", StaticFiles(directory=web_dist, html=True), name="web")

    return app
