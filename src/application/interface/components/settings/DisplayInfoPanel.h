#ifndef _DISPLAY_INFO_PANEL_H_
#define _DISPLAY_INFO_PANEL_H_

#include "application/Application.h"
#include "application/interface/components/types/StatefulComponent.h"

// Displays the actual screen dimensions read from the device at runtime.
class DisplayInfoPanel : public StatefulComponent {
  lv_obj_t *sizeLabel = nullptr;

public:
  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(lvObj, 4, 0);

    lv_obj_t *label = lv_label_create(lvObj);
    lv_label_set_text(label, "Display");
    lv_obj_set_style_text_color(label, lv_color_hex(0xA1A1AA), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);

    sizeLabel = lv_label_create(lvObj);
    lv_obj_set_style_text_color(sizeLabel, lv_color_hex(0xFAFAFA), 0);
    lv_obj_set_style_text_font(sizeLabel, &lv_font_montserrat_14, 0);

    update();
  }

  void update() override {
    if (sizeLabel == nullptr) return;
    if (app == nullptr) {
      lv_label_set_text(sizeLabel, "Unknown");
      return;
    }
    char buf[32];
    int w = app->device()->display().width();
    int h = app->device()->display().height();
    snprintf(buf, sizeof(buf), "%d x %d", w, h);
    lv_label_set_text(sizeLabel, buf);
  }
};

#endif // _DISPLAY_INFO_PANEL_H_
