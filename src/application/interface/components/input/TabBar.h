#ifndef _TAB_BAR_H_
#define _TAB_BAR_H_

#include <initializer_list>
#include <vector>

#include "application/Application.h"
#include "application/interface/components/input/Button.h"
#include "application/interface/components/types/Component.h"
#include "events/types/TouchEvent.h"

struct TabBarItem {
  const char *icon;  // LV_SYMBOL_* string
  const char *label;
  State state;
};

class TabBar : public Component {
  std::vector<TabBarItem> items;
  State activeState;
  std::vector<lv_obj_t *> tabObjs;
  Timer debounce{200};

public:
  TabBar(State activeState, std::initializer_list<TabBarItem> items)
      : items(items), activeState(activeState) {};

  void createWidgets(lv_obj_t *parent) override {
    int screenW = (app != nullptr) ? app->device()->display().width() : 240;
    bool useTabs = screenW >= 480;

    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_width(lvObj, LV_PCT(100));
    lv_obj_set_height(lvObj, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    if (useTabs) {
      // Full tab bar with icons + labels
      lv_obj_set_style_pad_hor(lvObj, 8, 0);
      lv_obj_set_style_pad_ver(lvObj, 6, 0);
      lv_obj_set_style_pad_column(lvObj, 4, 0);
      lv_obj_set_style_bg_color(lvObj, lv_color_hex(0x18181B), 0); // zinc-900
      lv_obj_set_style_bg_opa(lvObj, LV_OPA_COVER, 0);

      for (auto &item : items) {
        bool isActive = item.state == activeState;

        lv_obj_t *tab = lv_obj_create(lvObj);
        lv_obj_remove_style_all(tab);
        lv_obj_set_height(tab, LV_SIZE_CONTENT);
        lv_obj_set_flex_grow(tab, 1);
        lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(tab, LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_ver(tab, 6, 0);
        lv_obj_set_style_pad_column(tab, 6, 0);
        lv_obj_set_style_radius(tab, 8, 0);

        if (isActive) {
          lv_obj_set_style_bg_color(tab, lv_color_hex(0x27272A), 0); // zinc-800
          lv_obj_set_style_bg_opa(tab, LV_OPA_COVER, 0);
        }

        // Icon + label
        char buf[64];
        snprintf(buf, sizeof(buf), "%s %s", item.icon, item.label);
        lv_obj_t *lbl = lv_label_create(tab);
        lv_label_set_text(lbl, buf);
        lv_obj_set_style_text_color(lbl,
            lv_color_hex(isActive ? 0xFAFAFA : 0x71717A), 0);

        tabObjs.push_back(tab);
      }
    } else {
      // Dot indicators for small screens
      lv_obj_set_style_pad_ver(lvObj, 4, 0);
      lv_obj_set_style_pad_column(lvObj, 6, 0);

      for (auto &item : items) {
        bool isActive = item.state == activeState;

        lv_obj_t *dot = lv_obj_create(lvObj);
        lv_obj_remove_style_all(dot);
        lv_obj_set_size(dot, isActive ? 8 : 6, isActive ? 8 : 6);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dot,
            lv_color_hex(isActive ? 0xFAFAFA : 0x3F3F46), 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);

        tabObjs.push_back(dot);
      }
    }
  }

  void handleEvent(InputEvent &event) override {
    if (lvObj == nullptr) return;
    if (app == nullptr) return;
    if (event.inputType != InputType::TouchInput) return;
    TouchEvent &te = static_cast<TouchEvent &>(event);
    if (te.type != TouchType::TapType) return;

    TapTouchEvent &tap = static_cast<TapTouchEvent &>(event);

    if (debounce.running()) return;

    for (size_t i = 0; i < tabObjs.size() && i < items.size(); i++) {
      if (items[i].state == activeState) continue;

      if (Button::hitTest(tabObjs[i], tap.location.x, tap.location.y)) {
        debounce.start();
        app->workflow().navigate(items[i].state);
        return;
      }
    }
  }
};

#endif // _TAB_BAR_H_
