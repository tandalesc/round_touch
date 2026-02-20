"""Data source utility endpoints — template testing and entity exploration."""

import logging

import httpx
from fastapi import APIRouter, Query

from app.config import HAConfig
from app.services.datasources import DataSourceRegistry

logger = logging.getLogger(__name__)

router = APIRouter(prefix="/api/datasources", tags=["datasources"])


def create_router(datasources: DataSourceRegistry,
                  ha_config: HAConfig) -> APIRouter:

    @router.get("/resolve")
    async def resolve_template(
        template: str = Query(..., description="Template string with {{prefix:key}} placeholders"),
    ):
        """Test template resolution without saving anything."""
        resolved = await datasources.resolve_template(template)
        return {"template": template, "resolved": resolved}

    @router.get("/ha/entities")
    async def list_ha_entities(
        domain: str | None = Query(None, description="Filter by domain (e.g. weather, sensor, light)"),
        q: str | None = Query(None, description="Search entity IDs and friendly names"),
    ):
        """Browse available Home Assistant entities."""
        if not ha_config.url or not ha_config.token:
            return {"error": "Home Assistant not configured", "entities": []}

        try:
            transport = httpx.AsyncHTTPTransport(local_address="0.0.0.0")
            async with httpx.AsyncClient(timeout=10.0, transport=transport) as client:
                resp = await client.get(
                    f"{ha_config.url.rstrip('/')}/api/states",
                    headers={"Authorization": f"Bearer {ha_config.token}"},
                )
                if resp.status_code != 200:
                    return {"error": f"HA returned HTTP {resp.status_code}", "entities": []}
                states = resp.json()
        except Exception as e:
            logger.error(f"HA entity list error: {e}")
            return {"error": str(e), "entities": []}

        entities = []
        for s in states:
            eid = s.get("entity_id", "")
            attrs = s.get("attributes", {})
            friendly = attrs.get("friendly_name", "")
            ent_domain = eid.split(".")[0] if "." in eid else ""

            if domain and ent_domain != domain:
                continue
            if q:
                q_lower = q.lower()
                if q_lower not in eid.lower() and q_lower not in friendly.lower():
                    continue

            attr_keys = sorted(k for k in attrs if k != "friendly_name")

            entities.append({
                "entity_id": eid,
                "friendly_name": friendly,
                "state": s.get("state", "unknown"),
                "domain": ent_domain,
                "attributes": attr_keys,
            })

        entities.sort(key=lambda e: e["entity_id"])
        return {"count": len(entities), "entities": entities}

    @router.get("/ha/entity/{entity_id:path}")
    async def get_ha_entity(entity_id: str):
        """Get full details for a specific HA entity, including all attributes."""
        if not ha_config.url or not ha_config.token:
            return {"error": "Home Assistant not configured"}

        try:
            transport = httpx.AsyncHTTPTransport(local_address="0.0.0.0")
            async with httpx.AsyncClient(timeout=10.0, transport=transport) as client:
                resp = await client.get(
                    f"{ha_config.url.rstrip('/')}/api/states/{entity_id}",
                    headers={"Authorization": f"Bearer {ha_config.token}"},
                )
                if resp.status_code != 200:
                    return {"error": f"HA returned HTTP {resp.status_code}"}
                data = resp.json()
        except Exception as e:
            logger.error(f"HA entity detail error: {e}")
            return {"error": str(e)}

        attrs = data.get("attributes", {})
        return {
            "entity_id": entity_id,
            "state": data.get("state", "unknown"),
            "friendly_name": attrs.get("friendly_name", ""),
            "attributes": attrs,
            "template_refs": [
                f"ha:{entity_id}",
                *[f"ha:{entity_id}.{k}" for k in sorted(attrs) if k != "friendly_name"],
            ],
        }

    return router
