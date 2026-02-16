#ifndef _NAV_BUTTON_H_
#define _NAV_BUTTON_H_

#include "application/Application.h"
#include "application/interface/components/types/Component.h"
#include "events/types/TouchEvent.h"

class NavButton : public Component {
  State targetState;
  const char *label;
  uint32_t bg;
  uint32_t textColor;

public:
  NavButton(const char *label, State targetState,
            uint32_t bg = 0x27272A, uint32_t textColor = 0xFAFAFA)
      : targetState(targetState), label(label), bg(bg), textColor(textColor) {};

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_width(lvObj, LV_PCT(100));
    lv_obj_set_height(lvObj, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(lvObj, lv_color_hex(bg), 0);
    lv_obj_set_style_bg_opa(lvObj, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(lvObj, 12, 0);
    lv_obj_set_style_pad_ver(lvObj, 10, 0);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_t *txt = lv_label_create(lvObj);
    lv_label_set_text(txt, label);
    lv_obj_set_style_text_color(txt, lv_color_hex(textColor), 0);
  }

  void handleEvent(InputEvent &event) override {
    if (lvObj == nullptr) return;
    if (event.inputType != InputType::TouchInput) return;
    TouchEvent &te = static_cast<TouchEvent &>(event);
    if (te.type != TouchType::TapType) return;

    TapTouchEvent &tap = static_cast<TapTouchEvent &>(event);
    lv_area_t area;
    lv_obj_get_coords(lvObj, &area);
    if (tap.location.x >= area.x1 && tap.location.x <= area.x2 &&
        tap.location.y >= area.y1 && tap.location.y <= area.y2) {
      if (app != nullptr) {
        app->workflow().navigate(targetState);
      }
    }
  }
};

#endif // _NAV_BUTTON_H_
