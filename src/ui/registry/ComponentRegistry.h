#ifndef _COMPONENT_REGISTRY_H_
#define _COMPONENT_REGISTRY_H_

#include <unordered_map>
#include <string>
#include <vector>

#include <ArduinoJson.h>

#include "application/interface/components/types/Component.h"

using FactoryFn = Component *(*)(const JsonObject &node,
                                  std::vector<Component *> children);

class ComponentRegistry {
  std::unordered_map<std::string, FactoryFn> _factories;

public:
  void reg(const char *name, FactoryFn fn) { _factories[name] = fn; }

  Component *create(const char *type, const JsonObject &node,
                    std::vector<Component *> children) const {
    auto it = _factories.find(type);
    if (it == _factories.end()) {
      Serial.printf("[Registry] Unknown component: %s\n", type);
      return new Component();
    }
    return it->second(node, std::move(children));
  }

  bool has(const char *type) const { return _factories.count(type) > 0; }
};

#endif // _COMPONENT_REGISTRY_H_
