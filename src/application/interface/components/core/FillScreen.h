#ifndef _FILL_SCREEN_COMPONENT_H_
#define _FILL_SCREEN_COMPONENT_H_

#include "application/interface/components/types/ComponentWithChildren.h"

struct FillScreenProps {
  uint32_t color = 0x09090B; // zinc-950
  int pad = 0;
  int gap = 0;
};

class FillScreen : public ComponentWithChildren {
private:
  FillScreenProps props;

public:
  template <typename... T>
  FillScreen(FillScreenProps props, T *...children)
      : ComponentWithChildren(children...), props(props){};

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(lvObj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(lvObj, lv_color_hex(props.color), 0);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    if (props.pad > 0) {
      lv_obj_set_style_pad_all(lvObj, props.pad, 0);
    }
    if (props.gap > 0) {
      lv_obj_set_style_pad_row(lvObj, props.gap, 0);
    }
    lv_obj_clear_flag(lvObj, LV_OBJ_FLAG_SCROLLABLE);
    for (auto &child : children) {
      child->createWidgets(lvObj);
    }
  }
};

#endif // _FILL_SCREEN_COMPONENT_H_
