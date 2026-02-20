#ifndef _DYNAMIC_TEXT_H_
#define _DYNAMIC_TEXT_H_

#include "application/interface/components/dynamic/ServerTextBase.h"

// Displays server-resolved template text. No LLM involved.
// Templates use {{prefix:key}} placeholders resolved server-side.
// Example: "Temperature: {{ha:sensor.temperature}}°F | {{time:time}}"
class DynamicText : public ServerTextBase {
protected:
  const char *apiPath() override { return "/api/dynamic/text"; }

public:
  DynamicText(const char *contentKey, int ttl, uint8_t size, uint32_t color)
      : ServerTextBase(contentKey, ttl, size, color) {}
};

#endif // _DYNAMIC_TEXT_H_
