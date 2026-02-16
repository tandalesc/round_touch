#ifndef _BUTTON_COMPONENT_H_
#define _BUTTON_COMPONENT_H_

#include "application/interface/components/types/Component.h"
#include "events/types/TouchEvent.h"
#include "util/Timer.h"

typedef void (*ButtonCallback)(Application *app, void *userData);

struct ButtonProps {
  const char *label = "";
  uint32_t bg = 0x27272A;      // zinc-800
  uint32_t color = 0xFAFAFA;   // zinc-50
  int radius = 12;
  int padV = 10;
  int padH = 16;
  int debounceMs = 200;
};

class Button : public Component {
  ButtonProps props;
  ButtonCallback callback;
  void *userData;
  Timer debounce{200};

public:
  Button(ButtonProps props, ButtonCallback callback,
         void *userData = nullptr)
      : props(props), callback(callback), userData(userData),
        debounce(props.debounceMs) {}

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_width(lvObj, LV_PCT(100));
    lv_obj_set_height(lvObj, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(lvObj, lv_color_hex(props.bg), 0);
    lv_obj_set_style_bg_opa(lvObj, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(lvObj, props.radius, 0);
    lv_obj_set_style_pad_ver(lvObj, props.padV, 0);
    lv_obj_set_style_pad_hor(lvObj, props.padH, 0);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_t *txt = lv_label_create(lvObj);
    lv_label_set_text(txt, props.label);
    lv_obj_set_style_text_color(txt, lv_color_hex(props.color), 0);
  }

  void handleEvent(InputEvent &event) override {
    if (lvObj == nullptr) return;
    if (event.inputType != InputType::TouchInput) return;
    auto &te = static_cast<TouchEvent &>(event);
    if (te.type != TouchType::TapType) return;

    auto &tap = static_cast<TapTouchEvent &>(event);
    lv_area_t area;
    lv_obj_get_coords(lvObj, &area);
    if (tap.location.x < area.x1 || tap.location.x > area.x2 ||
        tap.location.y < area.y1 || tap.location.y > area.y2)
      return;

    if (debounce.running()) return;
    debounce.start();

    if (callback != nullptr && app != nullptr) {
      callback(app, userData);
    }
  }

  // Static hit-test utility for other components to reuse
  static bool hitTest(lv_obj_t *obj, int x, int y) {
    if (obj == nullptr) return false;
    lv_area_t area;
    lv_obj_get_coords(obj, &area);
    return x >= area.x1 && x <= area.x2 &&
           y >= area.y1 && y <= area.y2;
  }
};

#endif // _BUTTON_COMPONENT_H_
