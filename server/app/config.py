"""Configuration loading from TOML + CLI overrides."""

import tomllib
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class HAConfig:
    url: str = ""
    token: str = ""


@dataclass
class LLMModel:
    url: str = ""
    model_id: str = ""


@dataclass
class LLMConfig:
    models: dict[str, LLMModel] = field(default_factory=dict)
    default_model: str = "qwen3-8b"


@dataclass
class Config:
    firmware_dir: str = ""
    secret_key: str = ""
    version: str = ""
    port: int = 8080
    ui_dir: str = "ui"
    db_path: str = "round_touch.db"
    storage_dir: str = "storage"
    homeassistant: HAConfig = field(default_factory=HAConfig)
    llm: LLMConfig = field(default_factory=LLMConfig)


def load_config(config_path: str | Path) -> Config:
    """Load config from a TOML file, returning a Config dataclass."""
    cfg = Config()
    path = Path(config_path)
    if path.exists():
        with open(path, "rb") as f:
            data = tomllib.load(f)
        # Top-level keys
        for key in ("firmware_dir", "secret_key", "version", "port",
                     "ui_dir", "db_path", "storage_dir"):
            if key in data:
                setattr(cfg, key, data[key])
        # Home Assistant section
        if "homeassistant" in data:
            ha = data["homeassistant"]
            cfg.homeassistant = HAConfig(
                url=ha.get("url", ""),
                token=ha.get("token", ""),
            )
        # LLM section
        if "llm" in data:
            llm = data["llm"]
            models = {}
            for key, val in llm.get("models", {}).items():
                if isinstance(val, dict):
                    models[key] = LLMModel(
                        url=val.get("url", ""),
                        model_id=val.get("model_id", ""),
                    )
                else:
                    # Backwards compat: bare string = URL, model_id = key
                    models[key] = LLMModel(url=val, model_id=key)
            cfg.llm = LLMConfig(
                models=models,
                default_model=llm.get("default_model", "qwen3-8b"),
            )
    return cfg
