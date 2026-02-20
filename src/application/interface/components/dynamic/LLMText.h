#ifndef _LLM_TEXT_H_
#define _LLM_TEXT_H_

#include "application/interface/components/dynamic/ServerTextBase.h"

// Displays LLM-generated text from the server.
// The prompt template uses {{prefix:key}} placeholders which are
// resolved server-side before being sent to the LLM.
// Results are cached with a TTL on the server.
class LLMText : public ServerTextBase {
protected:
  const char *apiPath() override { return "/api/dynamic/llm"; }

public:
  LLMText(const char *contentKey, int ttl, uint8_t size, uint32_t color)
      : ServerTextBase(contentKey, ttl, size, color) {}
};

#endif // _LLM_TEXT_H_
