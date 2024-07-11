#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Arduino_GFX_Library.h>

#define TFT_BLK 45
#define TFT_RES 11

#define TFT_CS 15
#define TFT_MOSI 13
#define TFT_MISO 12
#define TFT_SCLK 14
#define TFT_DC 21

class Display {
private:
  Arduino_ESP32SPI *bus;
  Display() {
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, 1);
    bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO,
                               HSPI, true);
    gfx = new Arduino_GC9A01(bus, TFT_RES, 0, true);
    gfx->begin();
  }

public:
  Arduino_GFX *gfx;
  static Display &getInstance() {
    static Display display;
    return display;
  }
  ~Display() {
    delete gfx;
    delete bus;
  }
  Display(Display const &) = delete;
  void operator=(Display const &) = delete;
};

#endif // _DISPLAY_H_