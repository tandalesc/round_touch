"""LLM client for OpenAI-compatible endpoints."""

import logging

import httpx

from app.config import LLMConfig

logger = logging.getLogger(__name__)


class LLMService:
    def __init__(self, config: LLMConfig):
        self.models = config.models
        self.default_model = config.default_model

    def _get_model(self, model_key: str):
        """Return the LLMModel for a key, or None."""
        from app.config import LLMModel
        return self.models.get(model_key)

    async def generate(self, model_key: str, prompt: str,
                       max_tokens: int = 200) -> str:
        """Call an OpenAI-compatible chat completions endpoint."""
        key = model_key or self.default_model
        model = self._get_model(key)
        if not model:
            logger.error(f"Unknown LLM model: {key}")
            return f"[LLM error: unknown model '{key}']"

        endpoint = model.url.rstrip("/") + "/chat/completions"
        model_id = model.model_id or key

        logger.info(f"LLM request: endpoint={endpoint}, model_id={model_id}")

        try:
            transport = httpx.AsyncHTTPTransport(
                local_address="0.0.0.0",  # force IPv4
            )
            async with httpx.AsyncClient(timeout=60.0, transport=transport) as client:
                resp = await client.post(
                    endpoint,
                    json={
                        "model": model_id,
                        "messages": [{"role": "user", "content": prompt}],
                        "max_tokens": max_tokens,
                        "temperature": 0.7,
                    },
                )
                if resp.status_code != 200:
                    logger.error(f"LLM call failed: endpoint={endpoint}, model_id={model_id}, HTTP {resp.status_code} — {resp.text[:500]}")
                    return "[LLM error: request failed]"
                data = resp.json()
                choices = data.get("choices", [])
                if not choices:
                    return "[LLM error: no response]"
                return choices[0].get("message", {}).get("content", "").strip()
        except httpx.TimeoutException:
            logger.error(f"LLM call timed out for model {model_key}")
            return "[LLM error: timeout]"
        except Exception as e:
            logger.error(f"LLM call error: {e}")
            return f"[LLM error: {e}]"
