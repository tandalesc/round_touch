#ifndef _CARD_COMPONENT_H_
#define _CARD_COMPONENT_H_

#include "application/interface/components/types/ComponentWithChildren.h"

struct CardProps {
  uint32_t bg = 0x18181B;     // zinc-900
  uint32_t border = 0x27272A; // zinc-800
  int radius = 12;
  int pad = 12;
  int gap = 8;
};

class Card : public ComponentWithChildren {
private:
  CardProps props;

public:
  template <typename... T>
  Card(CardProps props, T *...children)
      : ComponentWithChildren(children...), props(props) {};

  template <typename... T>
  Card(T *...children) : Card({}, children...) {};

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_width(lvObj, LV_PCT(100));
    lv_obj_set_height(lvObj, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(lvObj, lv_color_hex(props.bg), 0);
    lv_obj_set_style_bg_opa(lvObj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(lvObj, lv_color_hex(props.border), 0);
    lv_obj_set_style_border_width(lvObj, 1, 0);
    lv_obj_set_style_border_opa(lvObj, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(lvObj, props.radius, 0);
    lv_obj_set_style_pad_all(lvObj, props.pad, 0);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(lvObj, props.gap, 0);

    for (auto &child : children) {
      child->createWidgets(lvObj);
    }
  }
};

#endif // _CARD_COMPONENT_H_
