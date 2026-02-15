#ifndef _HOME_ASSISTANT_H_
#define _HOME_ASSISTANT_H_

#include "device/INetwork.h"

class HomeAssistant {
private:
  INetwork *_network;
  const char *_baseUrl;
  const char *_token;
  // Prebuilt auth header: "Bearer <token>"
  String _authHeader;

public:
  HomeAssistant(INetwork *network, const char *baseUrl, const char *token);

  // Get the state string for an entity (e.g., "on", "off", "unavailable")
  String getEntityState(const char *entityId);

  // Toggle an entity. Returns true on success.
  bool toggle(const char *entityId);

  // Explicit on/off. Returns true on success.
  bool turnOn(const char *entityId);
  bool turnOff(const char *entityId);
};

#endif // _HOME_ASSISTANT_H_
