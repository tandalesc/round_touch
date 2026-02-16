#ifndef _GAUGE_CARD_H_
#define _GAUGE_CARD_H_

#include "application/interface/components/core/Card.h"
#include "application/interface/components/core/Text.h"
#include "application/interface/components/layout/FlexLayout.h"
#include "config/screens/Theme.h"

struct GaugeCardProps {
  const char *label = "";
  const char *value = "";
};

class GaugeCard : public Component {
  GaugeCardProps props;

public:
  GaugeCard(GaugeCardProps props) : props(props) {}

  void createWidgets(lv_obj_t *parent) override {
    // Outer card container
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_width(lvObj, LV_PCT(100));
    lv_obj_set_height(lvObj, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(lvObj, lv_color_hex(CLR_ZINC_900), 0);
    lv_obj_set_style_bg_opa(lvObj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(lvObj, lv_color_hex(CLR_ZINC_800), 0);
    lv_obj_set_style_border_width(lvObj, 1, 0);
    lv_obj_set_style_border_opa(lvObj, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(lvObj, 12, 0);
    lv_obj_set_style_pad_all(lvObj, 12, 0);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lvObj, 4, 0);

    // Label
    auto *lbl = lv_label_create(lvObj);
    lv_label_set_text(lbl, props.label);
    lv_obj_set_style_text_color(lbl, lv_color_hex(CLR_ZINC_400), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);

    // Value
    auto *val = lv_label_create(lvObj);
    lv_label_set_text(val, props.value);
    lv_obj_set_style_text_color(val, lv_color_hex(CLR_ZINC_50), 0);
    lv_obj_set_style_text_font(val, &lv_font_montserrat_14, 0);
  }
};

#endif // _GAUGE_CARD_H_
