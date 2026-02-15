#ifndef _TEXT_COMPONENT_H_
#define _TEXT_COMPONENT_H_

#include "application/interface/components/types/Component.h"

struct TextProps {
  uint8_t size = 4;
  uint32_t color = 0xFFFFFF;
};

class Text : public Component {
  TextProps props;
  const char *text = "";

public:
  Text(TextProps props, const char *text) : props(props), text(text) {};
  Text(const char *text) : Text({}, text) {};

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_label_create(parent);
    lv_label_set_text(lvObj, text);
    lv_obj_set_style_text_color(lvObj, lv_color_hex(props.color), 0);
  }
};

#endif
