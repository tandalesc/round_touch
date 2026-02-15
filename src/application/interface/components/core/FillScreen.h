#ifndef _FILL_SCREEN_COMPONENT_H_
#define _FILL_SCREEN_COMPONENT_H_

#include "application/interface/components/types/ComponentWithChildren.h"

struct FillScreenProps {
  uint32_t color = 0x000000;
};

class FillScreen : public ComponentWithChildren {
private:
  FillScreenProps props;

public:
  template <typename... T>
  FillScreen(FillScreenProps props, T *...children)
      : ComponentWithChildren(children...), props(props){};

  void createWidgets(lv_obj_t *parent) override {
    // FillScreen becomes the root container filling the screen
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(lvObj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(lvObj, lv_color_hex(props.color), 0);
    // create children inside this container
    for (auto &child : children) {
      child->createWidgets(lvObj);
    }
  }
};

#endif // _FILL_SCREEN_COMPONENT_H_
