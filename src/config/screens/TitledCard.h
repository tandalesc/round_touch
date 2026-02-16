#ifndef _TITLED_CARD_H_
#define _TITLED_CARD_H_

#include "application/interface/components/core/Card.h"
#include "application/interface/components/core/Text.h"
#include "config/screens/Theme.h"

struct TitledCardProps {
  const char *icon = "";
  const char *title = "";
  uint32_t bg = CLR_ZINC_900;
  uint32_t border = CLR_ZINC_800;
};

class TitledCard : public ComponentWithChildren {
  TitledCardProps props;
  char titleBuf[64] = {};

public:
  template <typename... T>
  TitledCard(TitledCardProps props, T *...children)
      : ComponentWithChildren(children...), props(props) {
    formatTitle();
  }

  TitledCard(TitledCardProps props, std::vector<RenderableComponent> kids)
      : ComponentWithChildren(std::move(kids)), props(props) {
    formatTitle();
  }

private:
  void formatTitle() {
    if (props.icon[0] != '\0') {
      snprintf(titleBuf, sizeof(titleBuf), "%s %s", props.icon, props.title);
    } else {
      snprintf(titleBuf, sizeof(titleBuf), "%s", props.title);
    }
  }

public:

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
    lv_obj_set_style_radius(lvObj, 12, 0);
    lv_obj_set_style_pad_all(lvObj, 12, 0);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(lvObj, 4, 0);

    // Title label
    auto *lbl = lv_label_create(lvObj);
    lv_label_set_text(lbl, titleBuf);
    lv_obj_set_style_text_color(lbl, lv_color_hex(CLR_ZINC_400), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);

    // Render children below the title
    for (auto &child : children) {
      child->createWidgets(lvObj);
    }
  }
};

#endif // _TITLED_CARD_H_
