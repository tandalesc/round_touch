#ifndef _SERVER_TEXT_BASE_H_
#define _SERVER_TEXT_BASE_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include "application/Application.h"
#include "application/interface/components/types/StatefulComponent.h"
#include "config/NetworkConfig.h"

#ifndef BOARD_SIMULATOR
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#endif

// Base class for components that fetch text content from the server.
// DynamicText and LLMText derive from this, differing only in their API path.
//
// The content_key is computed server-side and injected into the manifest JSON
// when saving. The device reads it directly — no client-side hashing needed.
//
// On ESP32 hardware: network fetches run in a FreeRTOS background task.
// On simulator: fetches run synchronously in the LVGL timer callback.
class ServerTextBase : public StatefulComponent {
protected:
  String contentKey;
  int ttlSeconds;
  uint8_t fontSize;
  uint32_t fontColor;

  String currentText;
  String etag;
  bool loading = true;

  lv_obj_t *textLabel = nullptr;
  lv_timer_t *pollTimer = nullptr;

  virtual const char *apiPath() = 0;

  void fetchContent() {
    if (app == nullptr) return;
    INetwork &net = app->device()->network();
    if (!net.isConnected()) {
      if (loading) {
        currentText = "No network";
        loading = false;
        update();
      }
      return;
    }

    char url[256];
    snprintf(url, sizeof(url), "%s%s?key=%s",
             OTA_UPDATE_URL, apiPath(), contentKey.c_str());

    const char *etagPtr = etag.length() > 0 ? etag.c_str() : nullptr;
    HttpResponse resp = net.get(url, nullptr, etagPtr);

    if (resp.statusCode == 304) {
      // Content unchanged
      if (loading) {
        loading = false;
        update();
      }
      return;
    }

    if (resp.statusCode == 200) {
      // Parse JSON response: {"text": "...", "etag": "..."}
      JsonDocument doc;
      DeserializationError err = deserializeJson(doc, resp.body);
      if (!err) {
        currentText = String(doc["text"].as<const char *>());
        if (doc["etag"]) {
          etag = String("\"") + String(doc["etag"].as<const char *>()) + "\"";
        }
      } else {
        currentText = "[parse error]";
      }
      loading = false;
      update();
      return;
    }

    if (resp.statusCode > 0) {
      Serial.printf("[ServerText] HTTP %d for key=%s\n",
                    resp.statusCode, contentKey.c_str());
    }
    if (loading) {
      currentText = "Error";
      loading = false;
      update();
    }
  }

#ifndef BOARD_SIMULATOR
  // --- FreeRTOS background fetch ---
  struct FetchContext {
    ServerTextBase *self;
    bool done = false;
    SemaphoreHandle_t mutex;
  };

  FetchContext *_fetchCtx = nullptr;

  static void fetchTask(void *param) {
    FetchContext *ctx = static_cast<FetchContext *>(param);
    ctx->self->fetchContent();
    xSemaphoreTake(ctx->mutex, portMAX_DELAY);
    ctx->done = true;
    xSemaphoreGive(ctx->mutex);
    vTaskDelete(nullptr);
  }

  void startBackgroundFetch() {
    if (_fetchCtx != nullptr) {
      // Check if previous fetch is still running
      xSemaphoreTake(_fetchCtx->mutex, portMAX_DELAY);
      bool done = _fetchCtx->done;
      xSemaphoreGive(_fetchCtx->mutex);
      if (!done) return;  // Still running, skip
      vSemaphoreDelete(_fetchCtx->mutex);
      delete _fetchCtx;
    }
    _fetchCtx = new FetchContext{this, false, xSemaphoreCreateMutex()};
    xTaskCreate(fetchTask, "srvtxt", 8192, _fetchCtx, 1, nullptr);
  }
#endif

  static void pollTimerCb(lv_timer_t *timer) {
    auto *self = static_cast<ServerTextBase *>(lv_timer_get_user_data(timer));
#ifdef BOARD_SIMULATOR
    self->fetchContent();
#else
    self->startBackgroundFetch();
#endif
  }

  static void deferredFetchCb(lv_timer_t *timer) {
    auto *self = static_cast<ServerTextBase *>(lv_timer_get_user_data(timer));
#ifdef BOARD_SIMULATOR
    self->fetchContent();
#else
    self->startBackgroundFetch();
#endif
  }

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
  ServerTextBase(const char *contentKey, int ttl, uint8_t size, uint32_t color)
      : contentKey(contentKey), ttlSeconds(ttl), fontSize(size),
        fontColor(color) {}

  virtual ~ServerTextBase() {
#ifndef BOARD_SIMULATOR
    if (_fetchCtx) {
      vSemaphoreDelete(_fetchCtx->mutex);
      delete _fetchCtx;
    }
#endif
    if (pollTimer) {
      lv_timer_delete(pollTimer);
    }
  }

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_SIZE_CONTENT);

    textLabel = lv_label_create(lvObj);
    lv_obj_set_width(textLabel, LV_PCT(100));
    lv_label_set_long_mode(textLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(textLabel, fontForSize(fontSize), 0);
    if (fontColor != 0) {
      lv_obj_set_style_text_color(textLabel, lv_color_hex(fontColor), 0);
    }

    loading = true;
    update();

    // Deferred initial fetch
    lv_timer_t *t = lv_timer_create(deferredFetchCb, 100, this);
    lv_timer_set_repeat_count(t, 1);

    // Repeating poll timer
    if (ttlSeconds > 0) {
      pollTimer = lv_timer_create(pollTimerCb, ttlSeconds * 1000, this);
    }
  }

  void update() override {
    if (textLabel == nullptr) return;
    if (loading) {
      lv_label_set_text(textLabel, LV_SYMBOL_REFRESH " Loading...");
    } else {
      lv_label_set_text(textLabel, currentText.c_str());
    }
  }
};

#endif // _SERVER_TEXT_BASE_H_
