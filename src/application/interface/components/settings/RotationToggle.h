#ifndef _ROTATION_TOGGLE_H_
#define _ROTATION_TOGGLE_H_

#include "application/interface/components/types/StatefulComponent.h"
#include "application/interface/components/input/Button.h"
#include "events/types/TouchEvent.h"

class RotationToggle : public StatefulComponent {
  lv_obj_t *valueLabel = nullptr;
  bool flipped = false;
  Timer debounce{200};

public:
  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(lvObj, 4, 0);

    lv_obj_t *label = lv_label_create(lvObj);
    lv_label_set_text(label, "Flip Display");
    lv_obj_set_style_text_color(label, lv_color_hex(0xFAFAFA), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);

    valueLabel = lv_label_create(lvObj);
    lv_obj_set_style_text_font(valueLabel, &lv_font_montserrat_14, 0);

    flipped = lv_display_get_rotation(NULL) == LV_DISPLAY_ROTATION_180;
    update();
  }

  void update() override {
    if (valueLabel == nullptr) return;
    lv_label_set_text(valueLabel, flipped ? "180\xC2\xB0" : "0\xC2\xB0");
    lv_obj_set_style_text_color(valueLabel, lv_color_hex(0x3B82F6), 0);
  }

  void handleEvent(InputEvent &event) override {
    if (event.inputType != InputType::TouchInput) return;
    TouchEvent &te = static_cast<TouchEvent &>(event);
    if (te.type != TouchType::TapType) return;

    TapTouchEvent &tap = static_cast<TapTouchEvent &>(event);
    if (!Button::hitTest(lvObj, tap.location.x, tap.location.y)) return;
    if (debounce.running()) return;
    debounce.start();

    flipped = !flipped;
    lv_display_set_rotation(NULL,
      flipped ? LV_DISPLAY_ROTATION_180 : LV_DISPLAY_ROTATION_0);
    update();
  }
};

#endif // _ROTATION_TOGGLE_H_
