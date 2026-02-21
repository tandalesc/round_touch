#ifndef _CONTENT_REFRESH_PANEL_H_
#define _CONTENT_REFRESH_PANEL_H_

#include "application/Application.h"
#include "application/interface/components/types/StatefulComponent.h"
#include "application/interface/components/input/Button.h"
#include "config/NetworkConfig.h"
#include "events/types/TouchEvent.h"

class ContentRefreshPanel : public StatefulComponent {
  enum class Status { Idle, Refreshing, Done, Error };
  Status status = Status::Idle;
  Timer debounce{500};

  lv_obj_t *statusLabel = nullptr;
  lv_obj_t *actionLabel = nullptr;

  void doRefresh() {
    if (app == nullptr) return;
    INetwork &net = app->device()->network();
    if (!net.isConnected()) {
      status = Status::Error;
      update();
      return;
    }

    status = Status::Refreshing;
    update();
    lv_refr_now(NULL);

    // 1. Invalidate server-side dynamic content cache
    char url[128];
    snprintf(url, sizeof(url), "%s/api/dynamic/refresh", OTA_UPDATE_URL);
    HttpResponse resp = net.post(url, "{}", "application/json");

    // 2. Re-fetch the UI manifest
    snprintf(url, sizeof(url), "%s/api/ui/screens?board=%s",
             OTA_UPDATE_URL, BOARD_ID);
    HttpResponse manifestResp = net.get(url);
    if (manifestResp.statusCode == 200) {
      app->userScreenManager().loadManifest(manifestResp.body.c_str());
    }

    status = (resp.statusCode == 200) ? Status::Done : Status::Error;
    update();
  }

public:
  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(lvObj, 8, 0);
    lv_obj_set_style_pad_row(lvObj, 4, 0);

    statusLabel = lv_label_create(lvObj);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_12, 0);

    actionLabel = lv_label_create(lvObj);
    lv_obj_set_style_text_font(actionLabel, &lv_font_montserrat_14, 0);

    update();
  }

  void update() override {
    if (actionLabel == nullptr) return;

    switch (status) {
    case Status::Idle:
      lv_label_set_text(statusLabel, "");
      lv_label_set_text(actionLabel, LV_SYMBOL_REFRESH " Refresh Content");
      lv_obj_set_style_text_color(actionLabel, lv_color_hex(0x3B82F6), 0);
      break;
    case Status::Refreshing:
      lv_label_set_text(statusLabel, "Refreshing...");
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xF59E0B), 0);
      lv_label_set_text(actionLabel, "");
      break;
    case Status::Done:
      lv_label_set_text(statusLabel, "Content refreshed");
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x22C55E), 0);
      lv_label_set_text(actionLabel, LV_SYMBOL_REFRESH " Refresh again");
      lv_obj_set_style_text_color(actionLabel, lv_color_hex(0x3B82F6), 0);
      break;
    case Status::Error:
      lv_label_set_text(statusLabel, "Refresh failed");
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xEF4444), 0);
      lv_label_set_text(actionLabel, LV_SYMBOL_REFRESH " Retry");
      lv_obj_set_style_text_color(actionLabel, lv_color_hex(0x3B82F6), 0);
      break;
    }
  }

  void handleEvent(InputEvent &event) override {
    if (event.inputType != InputType::TouchInput) return;
    TouchEvent &te = static_cast<TouchEvent &>(event);
    if (te.type != TouchType::TapType) return;
    if (app == nullptr || lvObj == nullptr) return;

    TapTouchEvent &tap = static_cast<TapTouchEvent &>(event);
    if (!Button::hitTest(lvObj, tap.location.x, tap.location.y)) return;
    if (debounce.running()) return;
    debounce.start();

    doRefresh();
  }
};

#endif // _CONTENT_REFRESH_PANEL_H_
