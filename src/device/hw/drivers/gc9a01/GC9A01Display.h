#ifndef _GC9A01_DISPLAY_H_
#define _GC9A01_DISPLAY_H_

#include <Arduino_GFX_Library.h>
#include "device/IDisplay.h"

// LVGL draw buffer size: 1/10th of screen
#define GC9A01_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10)

class GC9A01Display : public IDisplay {
private:
  Arduino_ESP32SPI *bus = nullptr;
  Arduino_GC9A01 *panel = nullptr;
  uint16_t *_buf = nullptr;

  static void flushCb(lv_display_t *disp, const lv_area_t *area, uint8_t *px) {
    auto *self = (GC9A01Display *)lv_display_get_user_data(disp);
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    self->panel->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)px, w, h);
    lv_display_flush_ready(disp);
  }

public:
  GC9A01Display() {}
  ~GC9A01Display() {
    free(_buf);
    delete panel;
    delete bus;
  }

  void init() override {
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, 1);
    bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO,
                               HSPI, true);
    panel = new Arduino_GC9A01(bus, TFT_RES, 0, true);
    panel->begin();
  }

  lv_display_t *initLVGL() override {
    lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_user_data(disp, this);
    lv_display_set_flush_cb(disp, flushCb);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    _buf = (uint16_t *)heap_caps_malloc(GC9A01_BUF_SIZE * sizeof(uint16_t),
                                         MALLOC_CAP_DMA);
    lv_display_set_buffers(disp, _buf, nullptr,
                           GC9A01_BUF_SIZE * sizeof(uint16_t),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    return disp;
  }

  int width() override { return SCREEN_WIDTH; }
  int height() override { return SCREEN_HEIGHT; }
};

#endif // _GC9A01_DISPLAY_H_
