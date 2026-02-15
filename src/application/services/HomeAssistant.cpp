#include "application/services/HomeAssistant.h"

#include <ArduinoJson.h>
#include <Arduino.h>

#include "config/NetworkConfig.h"

HomeAssistant::HomeAssistant(INetwork *network, const char *baseUrl,
                             const char *token)
    : _network(network), _baseUrl(baseUrl), _token(token) {
  _authHeader = String("Bearer ") + token;
}

String HomeAssistant::getEntityState(const char *entityId) {
  String url = String(_baseUrl) + "/api/states/" + entityId;
  HttpResponse resp = _network->get(url.c_str(), _authHeader.c_str());

  if (resp.statusCode != 200) {
    Serial.printf("[HA] GET state failed: %d\n", resp.statusCode);
    return "unavailable";
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, resp.body);
  if (err) {
    Serial.printf("[HA] JSON parse error: %s\n", err.c_str());
    return "unavailable";
  }

  return String(doc["state"].as<const char *>());
}

static bool callService(INetwork *network, const char *baseUrl,
                        const char *authHeader, const char *domain,
                        const char *service, const char *entityId) {
  String url = String(baseUrl) + "/api/services/" + domain + "/" + service;

  JsonDocument doc;
  doc["entity_id"] = entityId;
  String body;
  serializeJson(doc, body);

  HttpResponse resp =
      network->post(url.c_str(), body.c_str(), "application/json", authHeader);

  if (resp.statusCode != 200) {
    Serial.printf("[HA] %s/%s failed: %d\n", domain, service,
                  resp.statusCode);
    return false;
  }
  return true;
}

// Extract the domain from an entity_id (e.g., "light" from "light.living_room")
static String domainOf(const char *entityId) {
  String id(entityId);
  int dot = id.indexOf('.');
  if (dot < 0) return "homeassistant";
  return id.substring(0, dot);
}

bool HomeAssistant::toggle(const char *entityId) {
  return callService(_network, _baseUrl, _authHeader.c_str(),
                     domainOf(entityId).c_str(), "toggle", entityId);
}

bool HomeAssistant::turnOn(const char *entityId) {
  return callService(_network, _baseUrl, _authHeader.c_str(),
                     domainOf(entityId).c_str(), "turn_on", entityId);
}

bool HomeAssistant::turnOff(const char *entityId) {
  return callService(_network, _baseUrl, _authHeader.c_str(),
                     domainOf(entityId).c_str(), "turn_off", entityId);
}
