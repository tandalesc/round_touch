#ifndef _HA_BINARY_SENSOR_COMPONENT_H_
#define _HA_BINARY_SENSOR_COMPONENT_H_

#include "application/Application.h"
#include "application/interface/components/types/StatefulComponent.h"
#include "application/services/HomeAssistant.h"

// Displays a binary sensor (e.g. presence) with a friendly label.
// Shows a colored dot + "Home" / "Away" based on on/off state.
class HABinarySensor : public StatefulComponent {
  const char *entityId;
  const char *label;
  bool loading = true;
  String entityState = "unknown";

  lv_obj_t *nameLabel = nullptr;
  lv_obj_t *stateLabel = nullptr;

public:
  HABinarySensor(const char *entityId, const char *label)
      : entityId(entityId), label(label) {};

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(lvObj, 4, 0);

    nameLabel = lv_label_create(lvObj);
    lv_label_set_text(nameLabel, label);
    lv_obj_set_style_text_color(nameLabel, lv_color_hex(0xFAFAFA), 0);
    lv_obj_set_style_text_font(nameLabel, &lv_font_montserrat_14, 0);

    stateLabel = lv_label_create(lvObj);
    lv_obj_set_style_text_font(stateLabel, &lv_font_montserrat_12, 0);

    loading = true;
    update();

    lv_timer_t *t = lv_timer_create(deferredFetchCb, 100, this);
    lv_timer_set_repeat_count(t, 1);
  }

  void update() override {
    if (stateLabel == nullptr) return;
    if (loading) {
      lv_label_set_text(stateLabel, LV_SYMBOL_REFRESH);
      lv_obj_set_style_text_color(stateLabel, lv_color_hex(0x71717A), 0);
      return;
    }
    bool isHome = entityState == "on";
    lv_label_set_text(stateLabel,
                      isHome ? LV_SYMBOL_OK " Home" : LV_SYMBOL_CLOSE " Away");
    lv_obj_set_style_text_color(
        stateLabel, lv_color_hex(isHome ? 0x22C55E : 0xEF4444), 0);
  }

private:
  static void deferredFetchCb(lv_timer_t *timer) {
    auto *self = static_cast<HABinarySensor *>(lv_timer_get_user_data(timer));
    self->refreshState();
  }

  void refreshState() {
    if (app == nullptr) return;
    HomeAssistant *ha = app->ha();
    if (ha == nullptr) {
      loading = false;
      entityState = "unavailable";
      update();
      return;
    }
    entityState = ha->getEntityState(entityId);
    loading = false;
    update();
  }
};

#endif // _HA_BINARY_SENSOR_COMPONENT_H_
