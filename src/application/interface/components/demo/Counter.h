#ifndef _COUNTER_COMPONENT_H_
#define _COUNTER_COMPONENT_H_

#include "application/interface/components/types/StatefulComponent.h"
#include "events/types/TouchEvent.h"

class Counter : public StatefulComponent {
  int count = 0;
  lv_obj_t *label = nullptr;

public:
  void createWidgets(lv_obj_t *parent) override {
    label = lv_label_create(parent);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    update();
  }

  void update() override {
    if (label == nullptr) return;
    char buf[32];
    snprintf(buf, sizeof(buf), "Taps: %d", count);
    lv_label_set_text(label, buf);
  }

  void handleEvent(InputEvent &event) override {
    if (event.inputType != InputType::TouchInput) return;
    TouchEvent &te = static_cast<TouchEvent &>(event);
    if (te.type == TouchType::TapType) {
      count++;
      update();
    }
  }
};

#endif // _COUNTER_COMPONENT_H_
