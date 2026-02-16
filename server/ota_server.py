"""
OTA Update Server for round_touch firmware.

Serves firmware binaries, version metadata, and UI screen manifests.
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
import sys
import tomllib
from pathlib import Path

from fastapi import FastAPI, HTTPException, Query
from fastapi.responses import JSONResponse, StreamingResponse

app = FastAPI(title="round_touch OTA Server")

CONFIG = {
    "firmware_dir": "",
    "secret_key": "",
    "version": "",
    "port": 8080,
    "ui_dir": "ui",
}


def get_firmware_path(board: str) -> Path:
    return Path(CONFIG["firmware_dir"]) / board / "firmware.bin"


def compute_hmac(filepath: Path, secret_key: str) -> str:
    h = hmac.new(secret_key.encode(), digestmod=hashlib.sha256)
    with open(filepath, "rb") as f:
        while chunk := f.read(8192):
            h.update(chunk)
    return h.hexdigest()


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
