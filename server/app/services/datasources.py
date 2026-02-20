"""Pluggable data source registry for template resolution.

Data sources are namespaced with a prefix:
  - ha:weather.forecast_home  → Home Assistant entity state
  - time:now                  → Current date/time strings

Templates use {{prefix:key}} placeholders that get resolved by the
appropriate data source.
"""

import logging
import re
from abc import ABC, abstractmethod
from datetime import datetime, timezone

import httpx

from app.config import HAConfig

logger = logging.getLogger(__name__)

TEMPLATE_PATTERN = re.compile(r"\{\{(\w+):([^}]+)\}\}")


class DataSource(ABC):
    @abstractmethod
    async def resolve(self, key: str) -> str:
        """Resolve a key to a string value."""
        ...


class HomeAssistantSource(DataSource):
    def __init__(self, url: str, token: str):
        self.url = url.rstrip("/")
        self.token = token

    async def resolve(self, key: str) -> str:
        if not self.url or not self.token:
            return f"[ha:{key} unconfigured]"

        # Support dot-notation for attributes:
        #   ha:weather.forecast_home          → full summary of entity
        #   ha:weather.forecast_home.temperature → just the "temperature" attribute
        # HA entity IDs use exactly one dot (domain.object_id), so the first
        # dot is the domain separator and any subsequent dot is an attribute path.
        parts = key.split(".")
        if len(parts) >= 3:
            entity_id = f"{parts[0]}.{parts[1]}"
            attr_path = ".".join(parts[2:])
        else:
            entity_id = key
            attr_path = None

        try:
            transport = httpx.AsyncHTTPTransport(local_address="0.0.0.0")
            async with httpx.AsyncClient(timeout=10.0, transport=transport) as client:
                resp = await client.get(
                    f"{self.url}/api/states/{entity_id}",
                    headers={"Authorization": f"Bearer {self.token}"},
                )
                if resp.status_code != 200:
                    logger.warning(f"HA entity {entity_id}: HTTP {resp.status_code}")
                    return "unavailable"
                data = resp.json()

                if attr_path:
                    # Return a specific attribute value
                    attrs = data.get("attributes", {})
                    val = attrs.get(attr_path)
                    if val is None:
                        # Try state itself if attr_path matches
                        if attr_path == "state":
                            return str(data.get("state", "unknown"))
                        return "unavailable"
                    return str(val)

                # No attribute requested — build a summary
                state = data.get("state", "unknown")
                attrs = data.get("attributes", {})
                summary = [f"state={state}"]
                for k in ("temperature", "temperature_unit", "humidity",
                           "friendly_name", "unit_of_measurement"):
                    if k in attrs:
                        summary.append(f"{k}={attrs[k]}")
                return ", ".join(summary)
        except Exception as e:
            logger.error(f"HA resolve error for {entity_id}: {e}")
            return "unavailable"


class TimeSource(DataSource):
    async def resolve(self, key: str) -> str:
        now = datetime.now(timezone.utc).astimezone()
        if key == "now":
            return now.strftime("%A, %B %d, %Y %I:%M %p")
        elif key == "date":
            return now.strftime("%B %d, %Y")
        elif key == "time":
            return now.strftime("%I:%M %p")
        elif key == "hour":
            return str(now.hour)
        elif key == "weekday":
            return now.strftime("%A")
        else:
            return now.isoformat()


class DataSourceRegistry:
    def __init__(self):
        self.sources: dict[str, DataSource] = {}

    def register(self, prefix: str, source: DataSource):
        self.sources[prefix] = source

    async def resolve_template(self, template: str) -> str:
        """Replace all {{prefix:key}} placeholders with resolved values."""
        # Find all placeholders
        matches = TEMPLATE_PATTERN.findall(template)
        if not matches:
            return template

        result = template
        for prefix, key in matches:
            source = self.sources.get(prefix)
            if source is None:
                value = f"[unknown source: {prefix}]"
            else:
                value = await source.resolve(key)
            result = result.replace(f"{{{{{prefix}:{key}}}}}", value)

        return result


def create_registry(ha_config: HAConfig) -> DataSourceRegistry:
    """Create and populate a DataSourceRegistry from config."""
    registry = DataSourceRegistry()
    registry.register("time", TimeSource())
    if ha_config.url and ha_config.token:
        registry.register("ha", HomeAssistantSource(ha_config.url, ha_config.token))
    else:
        logger.warning("Home Assistant not configured — ha: data source unavailable")
    return registry
