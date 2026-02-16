#ifndef _HA_TOGGLE_COMPONENT_H_
#define _HA_TOGGLE_COMPONENT_H_

#include "application/Application.h"
#include "application/interface/components/types/StatefulComponent.h"
#include "application/interface/components/input/Button.h"
#include "application/services/HomeAssistant.h"
#include "events/types/TouchEvent.h"

class HAToggle : public StatefulComponent {
  const char *entityId;
  String entityState = "unknown";
  bool loading = true;
  lv_obj_t *nameLabel = nullptr;
  lv_obj_t *stateLabel = nullptr;
  Timer debounce{300};

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
    lv_obj_set_style_text_color(nameLabel, lv_color_hex(0xA1A1AA), 0); // zinc-400

    stateLabel = lv_label_create(lvObj);

    // Show loading state; defer the blocking HTTP fetch so
    // createWidgets() returns immediately and the screen renders first.
    loading = true;
    update();

    lv_timer_t *t = lv_timer_create(deferredFetchCb, 50, this);
    lv_timer_set_repeat_count(t, 1);
  }

  void update() override {
    if (stateLabel == nullptr) return;
    if (loading) {
      lv_label_set_text(stateLabel, LV_SYMBOL_REFRESH " Loading...");
      lv_obj_set_style_text_color(stateLabel, lv_color_hex(0x71717A), 0); // zinc-500
      return;
    }
    bool isOn = entityState == "on";
    lv_label_set_text(stateLabel, isOn ? LV_SYMBOL_OK " ON" : LV_SYMBOL_CLOSE " OFF");
    lv_obj_set_style_text_color(stateLabel,
                                lv_color_hex(isOn ? 0x22C55E : 0xEF4444), 0);
  }

  void handleEvent(InputEvent &event) override {
    if (lvObj == nullptr) return;
    if (event.inputType != InputType::TouchInput) return;
    TouchEvent &te = static_cast<TouchEvent &>(event);
    if (te.type != TouchType::TapType) return;

    TapTouchEvent &tap = static_cast<TapTouchEvent &>(event);
    if (!Button::hitTest(lvObj, tap.location.x, tap.location.y)) return;
    if (debounce.running()) return;
    debounce.start();

    if (app == nullptr) return;
    HomeAssistant *ha = app->ha();
    if (ha == nullptr) return;

    loading = true;
    update();
    lv_timer_handler();

    ha->toggle(entityId);
    refreshState();
  }

private:
  static void deferredFetchCb(lv_timer_t *timer) {
    auto *self = static_cast<HAToggle *>(lv_timer_get_user_data(timer));
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

#endif // _HA_TOGGLE_COMPONENT_H_
