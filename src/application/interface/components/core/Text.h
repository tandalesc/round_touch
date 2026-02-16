#ifndef _TEXT_COMPONENT_H_
#define _TEXT_COMPONENT_H_

#include "application/interface/components/types/Component.h"

// Size scale: 1=10px, 2=12px, 3=14px (default body), 4=18px, 5=24px
struct TextProps {
  uint8_t size = 3;
  uint32_t color = 0xFAFAFA; // zinc-50
};

class Text : public Component {
  TextProps props;
  const char *text = "";

  static const lv_font_t *fontForSize(uint8_t size) {
    switch (size) {
    case 1: return &lv_font_montserrat_10;
    case 2: return &lv_font_montserrat_12;
    case 3: return &lv_font_montserrat_14;
    case 4: return &lv_font_montserrat_18;
    case 5: return &lv_font_montserrat_24;
    default: return &lv_font_montserrat_14;
    }
  }

public:
  Text(TextProps props, const char *text) : props(props), text(text) {};
  Text(const char *text) : Text({}, text) {};

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_label_create(parent);
    lv_label_set_text(lvObj, text);
    lv_obj_set_style_text_color(lvObj, lv_color_hex(props.color), 0);
    lv_obj_set_style_text_font(lvObj, fontForSize(props.size), 0);
  }
};

#endif
