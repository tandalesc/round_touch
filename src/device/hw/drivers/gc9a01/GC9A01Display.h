#ifndef _GC9A01_DISPLAY_H_
#define _GC9A01_DISPLAY_H_

#include <Arduino_GFX_Library.h>
#include "device/IDisplay.h"

class GC9A01Display : public IDisplay {
private:
  Arduino_ESP32SPI *bus = nullptr;
  Arduino_GC9A01 *panel = nullptr;

public:
  GC9A01Display() {}
  ~GC9A01Display() {
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

  Arduino_GFX *gfx() override { return panel; }
  int width() override { return SCREEN_WIDTH; }
  int height() override { return SCREEN_HEIGHT; }
};

#endif // _GC9A01_DISPLAY_H_
