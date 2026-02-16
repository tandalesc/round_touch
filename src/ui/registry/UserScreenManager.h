#ifndef _USER_SCREEN_MANAGER_H_
#define _USER_SCREEN_MANAGER_H_

#include <unordered_map>
#include <vector>

#include <ArduinoJson.h>

#include "application/workflow/Workflow.h"
#include "ui/registry/ComponentRegistry.h"
#include "ui/registry/JsonTreeParser.h"

class UserScreenManager {
public:
  struct TabDef {
    State id;
    String icon;
    String label;
  };

private:
  // stateId → serialized screen JSON (re-serialized from manifest)
  std::unordered_map<int, String> _screenJsons;
  std::vector<TabDef> _tabs;
  State _defaultScreen = USER_STATE_BASE;
  // Keeps parsed JSON alive so const char* pointers remain valid
  // until the next screen build or manifest reload.
  JsonDocument _activeDoc;
  bool _loaded = false;

public:
  bool loadManifest(const char *json) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
      Serial.printf("[UserScreenManager] JSON parse error: %s\n",
                    err.c_str());
      return false;
    }

    _screenJsons.clear();
    _tabs.clear();

    _defaultScreen = doc["default_screen"] | (int)USER_STATE_BASE;

    // Parse tabs array
    JsonArray tabArr = doc["tabs"];
    if (tabArr) {
      for (JsonObject tab : tabArr) {
        TabDef td;
        td.id    = tab["id"] | (int)USER_STATE_BASE;
        td.icon  = tab["icon"] | "";
        td.label = tab["label"] | "";
        _tabs.push_back(std::move(td));
      }
    }

    // Store each screen's JSON as a serialized string
    JsonObject screens = doc["screens"];
    if (screens) {
      for (JsonPair kv : screens) {
        int stateId = atoi(kv.key().c_str());
        String serialized;
        serializeJson(kv.value(), serialized);
        _screenJsons[stateId] = std::move(serialized);
      }
    }

    _loaded = !_tabs.empty() && !_screenJsons.empty();
    if (_loaded) {
      Serial.printf("[UserScreenManager] Loaded %d tabs, %d screens\n",
                    (int)_tabs.size(), (int)_screenJsons.size());
    }
    return _loaded;
  }

  bool hasScreen(State state) const {
    return _screenJsons.count(state) > 0;
  }

  Component *buildScreen(State state, const ComponentRegistry &registry) {
    auto it = _screenJsons.find(state);
    if (it == _screenJsons.end()) return nullptr;

    // Copy the stored JSON before parsing — deserializeJson mutates
    // the input string in zero-copy mode, which would corrupt our store.
    String jsonCopy = it->second;

    _activeDoc.clear();
    DeserializationError err = deserializeJson(_activeDoc, jsonCopy);
    if (err) {
      Serial.printf("[UserScreenManager] Screen %d parse error: %s\n",
                    state, err.c_str());
      return nullptr;
    }

    JsonObject root = _activeDoc.as<JsonObject>();
    return JsonTreeParser::build(root, registry);
  }

  const std::vector<TabDef> &tabs() const { return _tabs; }
  State defaultScreen() const { return _defaultScreen; }
  bool isLoaded() const { return _loaded; }
};

#endif // _USER_SCREEN_MANAGER_H_
