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

String HomeAssistant::getEntityAttribute(const char *entityId,
                                         const char *attrKey) {
  String url = String(_baseUrl) + "/api/states/" + entityId;
  HttpResponse resp = _network->get(url.c_str(), _authHeader.c_str());

  if (resp.statusCode != 200) {
    Serial.printf("[HA] GET attr failed: %d\n", resp.statusCode);
    return "";
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, resp.body);
  if (err) {
    Serial.printf("[HA] JSON parse error: %s\n", err.c_str());
    return "";
  }

  JsonVariant attr = doc["attributes"][attrKey];
  if (attr.isNull()) return "";

  // Return numeric values as strings too
  if (attr.is<const char *>()) return String(attr.as<const char *>());
  if (attr.is<float>()) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", attr.as<float>());
    return String(buf);
  }
  if (attr.is<int>()) return String(attr.as<int>());
  return "";
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
