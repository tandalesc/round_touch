"""Entrypoint for the round_touch server.

Usage:
    python main.py --config server.toml
"""

import argparse
import logging
import sys
from pathlib import Path

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(name)s: %(message)s",
)
logger = logging.getLogger(__name__)


def main():
    import uvicorn
    from app.config import load_config
    from app import create_app

    parser = argparse.ArgumentParser(description="round_touch Server")
    parser.add_argument("--config", default="server.toml", help="TOML config file")
    parser.add_argument("--firmware-dir", help="Override firmware directory")
    parser.add_argument("--secret-key", help="Override HMAC secret key")
    parser.add_argument("--version", help="Override firmware version string")
    parser.add_argument("--port", type=int, help="Override listen port")
    args = parser.parse_args()

    config = load_config(args.config)

    # CLI overrides
    if args.firmware_dir:
        config.firmware_dir = args.firmware_dir
    if args.secret_key:
        config.secret_key = args.secret_key
    if args.version:
        config.version = args.version
    if args.port:
        config.port = args.port

    # Validate required config
    if not config.firmware_dir:
        logger.error("firmware_dir is required (set in config or --firmware-dir)")
        sys.exit(1)
    if not config.secret_key:
        logger.error("secret_key is required (set in config or --secret-key)")
        sys.exit(1)
    if not config.version:
        logger.error("version is required (set in config or --version)")
        sys.exit(1)

    logger.info(f"round_touch Server v{config.version}")
    logger.info(f"  Firmware dir: {Path(config.firmware_dir).resolve()}")
    logger.info(f"  UI dir: {Path(config.ui_dir).resolve()}")
    logger.info(f"  Database: {config.db_path}")
    logger.info(f"  Storage: {Path(config.storage_dir).resolve()}")
    logger.info(f"  Port: {config.port}")

    if config.homeassistant.url:
        logger.info(f"  Home Assistant: {config.homeassistant.url}")
    if config.llm.models:
        logger.info(f"  LLM models: {', '.join(config.llm.models.keys())}")

    app = create_app(config)
    uvicorn.run(app, host="0.0.0.0", port=config.port)


if __name__ == "__main__":
    main()
