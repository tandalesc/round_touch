#ifndef _JSON_TREE_PARSER_H_
#define _JSON_TREE_PARSER_H_

#include <vector>

#include <ArduinoJson.h>

#include "ui/registry/ComponentRegistry.h"

// Recursive JSON → Component* builder.
// Walks {"type", "props", "children"} nodes, looks up types in
// the ComponentRegistry, recurses on children, and returns a tree.

class JsonTreeParser {
public:
  static Component *build(const JsonObject &node,
                          const ComponentRegistry &registry) {
    const char *type = node["type"] | (const char *)nullptr;
    if (type == nullptr) {
      Serial.println("[JsonTreeParser] Node missing \"type\" key");
      return new Component();
    }

    // Recurse on children first (depth-first)
    std::vector<Component *> kids;
    JsonArray childArr = node["children"];
    if (childArr) {
      for (JsonObject child : childArr) {
        Component *c = build(child, registry);
        if (c != nullptr) kids.push_back(c);
      }
    }

    return registry.create(type, node, std::move(kids));
  }
};

#endif // _JSON_TREE_PARSER_H_
