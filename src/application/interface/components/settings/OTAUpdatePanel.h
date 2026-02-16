#ifndef _OTA_UPDATE_PANEL_H_
#define _OTA_UPDATE_PANEL_H_

#include "application/Application.h"
#include "application/interface/components/types/StatefulComponent.h"
#include "application/interface/Toast.h"
#include "application/services/OTAUpdate.h"
#include "config/Version.h"
#include "events/types/TouchEvent.h"

class OTAUpdatePanel : public StatefulComponent {
  OTAStatus otaStatus = OTAStatus::Idle;
  String availableVersion;

  lv_obj_t *versionLabel = nullptr;
  lv_obj_t *statusLabel = nullptr;
  lv_obj_t *progressBar = nullptr;
  lv_obj_t *actionLabel = nullptr;

public:
  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(lvObj, 8, 0);
    lv_obj_set_style_pad_row(lvObj, 6, 0);

    versionLabel = lv_label_create(lvObj);
    lv_obj_set_style_text_color(versionLabel, lv_color_hex(0xA1A1AA), 0); // zinc-400

    statusLabel = lv_label_create(lvObj);

    // Indeterminate progress bar — visible during busy states
    progressBar = lv_bar_create(lvObj);
    lv_obj_set_size(progressBar, LV_PCT(80), 4);
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(0x3F3F46), 0); // zinc-700
    lv_obj_set_style_bg_opa(progressBar, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(progressBar, lv_color_hex(0x3B82F6),
                              LV_PART_INDICATOR); // blue-500
    lv_obj_set_style_bg_opa(progressBar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(progressBar, 2, 0);
    lv_obj_set_style_radius(progressBar, 2, LV_PART_INDICATOR);
    lv_bar_set_range(progressBar, 0, 100);
    lv_obj_add_flag(progressBar, LV_OBJ_FLAG_HIDDEN);

    actionLabel = lv_label_create(lvObj);

    update();
  }

  void update() override {
    if (versionLabel == nullptr) return;

    char buf[64];
    snprintf(buf, sizeof(buf), "v%s", FIRMWARE_VERSION);
    lv_label_set_text(versionLabel, buf);

    bool busy = (otaStatus == OTAStatus::Checking ||
                 otaStatus == OTAStatus::Downloading ||
                 otaStatus == OTAStatus::Verifying);

    if (busy) {
      lv_obj_clear_flag(progressBar, LV_OBJ_FLAG_HIDDEN);
      if (otaStatus == OTAStatus::Downloading) {
        lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);
      } else {
        // Checking/Verifying: indeterminate, show full bar
        lv_bar_set_value(progressBar, 100, LV_ANIM_OFF);
      }
    } else {
      lv_obj_add_flag(progressBar, LV_OBJ_FLAG_HIDDEN);
    }

    switch (otaStatus) {
    case OTAStatus::Idle:
      lv_label_set_text(statusLabel, "");
      lv_label_set_text(actionLabel, "Check for Updates");
      lv_obj_set_style_text_color(actionLabel, lv_color_hex(0x3B82F6), 0); // blue-500
      break;

    case OTAStatus::Checking:
      lv_label_set_text(statusLabel, "Checking...");
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xF59E0B), 0); // amber-500
      lv_label_set_text(actionLabel, "");
      break;

    case OTAStatus::NoUpdate:
      lv_label_set_text(statusLabel, "Up to date");
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x22C55E), 0); // green-500
      lv_label_set_text(actionLabel, "Check again");
      lv_obj_set_style_text_color(actionLabel, lv_color_hex(0x3B82F6), 0); // blue-500
      break;

    case OTAStatus::UpdateAvailable:
      snprintf(buf, sizeof(buf), "v%s available", availableVersion.c_str());
      lv_label_set_text(statusLabel, buf);
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xF59E0B), 0); // amber-500
      lv_label_set_text(actionLabel, "Tap to Install");
      lv_obj_set_style_text_color(actionLabel, lv_color_hex(0x22C55E), 0); // green-500
      break;

    case OTAStatus::Downloading:
      lv_label_set_text(statusLabel, "Downloading...");
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xF59E0B), 0); // amber-500
      lv_label_set_text(actionLabel, "");
      break;

    case OTAStatus::Verifying:
      lv_label_set_text(statusLabel, "Verifying...");
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xF59E0B), 0); // amber-500
      lv_label_set_text(actionLabel, "");
      break;

    case OTAStatus::Success:
      lv_label_set_text(statusLabel, "Update complete!");
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x22C55E), 0); // green-500
      lv_label_set_text(actionLabel, "Rebooting...");
      lv_obj_set_style_text_color(actionLabel, lv_color_hex(0xA1A1AA), 0); // zinc-400
      break;

    case OTAStatus::Error:
      lv_label_set_text(statusLabel, "Update failed");
      lv_obj_set_style_text_color(statusLabel, lv_color_hex(0xEF4444), 0); // red-500
      lv_label_set_text(actionLabel, "Tap to retry");
      lv_obj_set_style_text_color(actionLabel, lv_color_hex(0x3B82F6), 0); // blue-500
      break;
    }
  }

  void handleEvent(InputEvent &event) override {
    if (event.inputType != InputType::TouchInput) return;
    TouchEvent &te = static_cast<TouchEvent &>(event);
    if (te.type != TouchType::TapType) return;
    if (app == nullptr) return;
    if (actionLabel == nullptr) return;

    // Only respond to taps on the action label (hit-test)
    TapTouchEvent &tap = static_cast<TapTouchEvent &>(event);
    lv_area_t area;
    lv_obj_get_coords(lvObj, &area);
    if (tap.location.x < area.x1 || tap.location.x > area.x2 ||
        tap.location.y < area.y1 || tap.location.y > area.y2) return;

    OTAUpdate *ota = app->ota();
    if (ota == nullptr) {
      otaStatus = OTAStatus::Error;
      update();
      return;
    }

    if (otaStatus == OTAStatus::Idle || otaStatus == OTAStatus::NoUpdate ||
        otaStatus == OTAStatus::Error) {
      otaStatus = OTAStatus::Checking;
      update();
      lv_refr_now(NULL); // flush frame before blocking network call

      bool hasUpdate = ota->checkForUpdate();

      if (hasUpdate) {
        otaStatus = OTAStatus::UpdateAvailable;
        availableVersion = ota->availableVersion();
      } else {
        otaStatus = ota->status();
      }
      update();
      lv_refr_now(NULL);

    } else if (otaStatus == OTAStatus::UpdateAvailable) {
      otaStatus = OTAStatus::Downloading;
      update();
      lv_refr_now(NULL); // flush "Downloading..." frame before blocking

      // No display refresh during download — forcing a render mid-download
      // crashes the ESP32 due to DMA contention (display + WiFi both use
      // PSRAM DMA). Progress is logged to serial only.
      bool success = ota->performUpdate();

      otaStatus = ota->status();
      update();
      lv_refr_now(NULL);

      if (success) {
#ifndef BOARD_SIMULATOR
        delay(1500);
        ESP.restart();
#endif
      }
    }
  }
};

#endif // _OTA_UPDATE_PANEL_H_
