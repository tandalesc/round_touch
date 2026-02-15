#include "lvgl.h"
#include "device/Device.h"

#ifdef BOARD_MAKERFABS_ROUND_128
#include "device/hw/drivers/gc9a01/GC9A01Display.h"
#include "device/hw/drivers/cst816s/CST816STouch.h"
#include "device/hw/drivers/sdcard/SPISDCard.h"
#elif defined(BOARD_WAVESHARE_S3_LCD_7)
#include "device/hw/drivers/ch422g/CH422G.h"
#include "device/hw/drivers/rgb_panel/RGBPanelDisplay.h"
#include "device/hw/drivers/gt911/GT911Touch.h"
#include "device/hw/drivers/sdcard/NoStorage.h"
#elif defined(BOARD_SIMULATOR)
#include "SimDrivers.h"
#endif

Device::Device() {
#ifdef BOARD_MAKERFABS_ROUND_128
  _display = new GC9A01Display();
  _touch = new CST816STouch();
  _storage = new SPISDCard();
#elif defined(BOARD_WAVESHARE_S3_LCD_7)
  _display = new RGBPanelDisplay();
  _touch = new GT911Touch();
  _storage = new NoStorage();
#elif defined(BOARD_SIMULATOR)
  _display = new SimDisplayDriver();
  _touch = new SimTouchDriver();
  _storage = new SimStorageDriver();
#endif
}

Device::~Device() {
  delete _storage;
  delete _touch;
  delete _display;
}

void Device::init() {
#ifdef BOARD_WAVESHARE_S3_LCD_7
  // CH422G must be initialized first - it controls reset pins for LCD and touch
  CH422G::init();
  CH422G::setAllOutput();
  CH422G::setPin(CH422G_EXIO_LCD_RST, true);
  CH422G::setPin(CH422G_EXIO_TP_RST, true);
  CH422G::setPin(CH422G_EXIO_LCD_BL, true);
  delay(100);
#endif

  _display->init();
  _storage->init();
  _touch->init();

  // Initialize LVGL and create the display driver
  lv_init();
#ifndef BOARD_SIMULATOR
  // Provide Arduino millis() as LVGL's tick source (SDL driver handles its own)
  lv_tick_set_cb((lv_tick_get_cb_t)millis);
#endif
  _display->initLVGL();

  if (_storage->hasError()) {
    Serial.println("SD Card initialization error");
  }
}

IDisplay &Device::display() { return *_display; }
ITouch &Device::touchscreen() { return *_touch; }
IStorage &Device::sdcard() { return *_storage; }
