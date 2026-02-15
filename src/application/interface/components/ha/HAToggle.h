#ifndef _HA_TOGGLE_COMPONENT_H_
#define _HA_TOGGLE_COMPONENT_H_

#include "application/Application.h"
#include "application/interface/components/types/StatefulComponent.h"
#include "application/services/HomeAssistant.h"
#include "events/types/TouchEvent.h"

class HAToggle : public StatefulComponent {
  const char *entityId;
  String entityState = "unknown";
  lv_obj_t *nameLabel = nullptr;
  lv_obj_t *stateLabel = nullptr;

public:
  HAToggle(const char *entityId) : entityId(entityId) {};

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(lvObj, 8, 0);

    nameLabel = lv_label_create(lvObj);
    lv_label_set_text(nameLabel, entityId);
    lv_obj_set_style_text_color(nameLabel, lv_color_hex(0xAAAAAA), 0);

    stateLabel = lv_label_create(lvObj);

    // Fetch initial state
    refreshState();
  }

  void update() override {
    if (stateLabel == nullptr) return;
    bool isOn = entityState == "on";
    lv_label_set_text(stateLabel, isOn ? "ON" : "OFF");
    lv_obj_set_style_text_color(stateLabel,
                                lv_color_hex(isOn ? 0x00FF00 : 0xFF4444), 0);
  }

  void handleEvent(InputEvent &event) override {
    if (event.inputType != InputType::TouchInput) return;
    TouchEvent &te = static_cast<TouchEvent &>(event);
    if (te.type != TouchType::TapType) return;

    if (app == nullptr) return;
    HomeAssistant *ha = app->ha();
    if (ha == nullptr) return;

    ha->toggle(entityId);
    refreshState();
  }

private:
  void refreshState() {
    if (app == nullptr) return;
    HomeAssistant *ha = app->ha();
    if (ha == nullptr) return;

    entityState = ha->getEntityState(entityId);
    update();
  }
};

#endif // _HA_TOGGLE_COMPONENT_H_
