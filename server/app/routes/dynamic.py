"""Dynamic content endpoints — template resolution and LLM-powered text."""

import hashlib
import logging

from fastapi import APIRouter, HTTPException, Header, Query, Response

from app.database import Database
from app.services.datasources import DataSourceRegistry
from app.services.llm import LLMService

logger = logging.getLogger(__name__)

router = APIRouter(prefix="/api/dynamic", tags=["dynamic"])


def create_router(db: Database, datasources: DataSourceRegistry,
                  llm: LLMService) -> APIRouter:

    @router.get("/text")
    async def dynamic_text(
        key: str = Query(..., description="Content key"),
        if_none_match: str | None = Header(None, alias="If-None-Match"),
    ):
        """Resolve a DynamicText template — no LLM, just data source substitution."""
        config = await db.get_dynamic_config(key)
        if not config:
            raise HTTPException(404, f"Unknown content key: {key}")
        if config["component_type"] != "DynamicText":
            raise HTTPException(400, f"Content key is not a DynamicText")

        # Resolve template placeholders
        resolved = _sanitize_for_lvgl(await datasources.resolve_template(config["template"]))

        # Compute ETag from resolved text
        etag = hashlib.sha256(resolved.encode()).hexdigest()[:16]

        if if_none_match and if_none_match.strip('"') == etag:
            return Response(status_code=304)

        return Response(
            content=f'{{"text":{_json_str(resolved)},"etag":"{etag}"}}',
            media_type="application/json",
            headers={"ETag": f'"{etag}"'},
        )

    @router.get("/llm")
    async def dynamic_llm(
        key: str = Query(..., description="Content key"),
        if_none_match: str | None = Header(None, alias="If-None-Match"),
    ):
        """Resolve an LLMText prompt and return cached or freshly generated text."""
        config = await db.get_dynamic_config(key)
        if not config:
            raise HTTPException(404, f"Unknown content key: {key}")
        if config["component_type"] != "LLMText":
            raise HTTPException(400, f"Content key is not an LLMText")

        # Check cache first
        cached = await db.get_cached_content(key)
        if cached and not cached["expired"]:
            if if_none_match and if_none_match.strip('"') == cached["etag"]:
                return Response(status_code=304)
            return Response(
                content=f'{{"text":{_json_str(cached["text"])},"etag":"{cached["etag"]}"}}',
                media_type="application/json",
                headers={"ETag": f'"{cached["etag"]}"'},
            )

        # Cache miss or expired — resolve template and call LLM
        resolved_prompt = await datasources.resolve_template(config["template"])
        logger.info(f"LLM call for key={key}, model={config['model']}")
        generated = _sanitize_for_lvgl(await llm.generate(config["model"], resolved_prompt))

        # Don't cache error responses — let the next request retry
        is_error = generated.startswith("[LLM error:")
        etag = hashlib.sha256(generated.encode()).hexdigest()[:16]
        if not is_error:
            await db.set_cached_content(key, generated, etag, config["ttl_seconds"])

        if if_none_match and if_none_match.strip('"') == etag:
            return Response(status_code=304)

        return Response(
            content=f'{{"text":{_json_str(generated)},"etag":"{etag}"}}',
            media_type="application/json",
            headers={"ETag": f'"{etag}"'},
        )

    @router.post("/refresh")
    async def refresh_content():
        """Invalidate all cached dynamic content, forcing fresh resolution on next request."""
        await db.invalidate_all_cached_content()
        return {"status": "ok"}

    return router


def _sanitize_for_lvgl(text: str) -> str:
    """Replace Unicode characters that LVGL's Montserrat fonts can't render.

    LVGL built-in fonts only cover basic Latin + a handful of symbols.
    Smart quotes, em-dashes, and emoji all render as boxes.
    """
    replacements = {
        "\u2018": "'",   # left single curly quote
        "\u2019": "'",   # right single curly quote (smart apostrophe)
        "\u201C": '"',   # left double curly quote
        "\u201D": '"',   # right double curly quote
        "\u2013": "-",   # en dash
        "\u2014": "-",   # em dash
        "\u2026": "...", # ellipsis
        "\u00B0": "°",   # degree (keep — in Montserrat Latin-1 supplement)
    }
    for old, new in replacements.items():
        text = text.replace(old, new)
    # Strip remaining non-ASCII (emoji, CJK, etc.) that LVGL can't render
    text = text.encode("ascii", errors="ignore").decode("ascii")
    return text


def _json_str(s: str) -> str:
    """JSON-encode a string value (with proper escaping)."""
    import json
    return json.dumps(s)
