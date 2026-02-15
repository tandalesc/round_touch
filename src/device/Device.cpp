#include "lvgl.h"
#include "device/Device.h"

#ifdef BOARD_MAKERFABS_ROUND_128
#include "device/hw/drivers/gc9a01/GC9A01Display.h"
#include "device/hw/drivers/cst816s/CST816STouch.h"
#include "device/hw/drivers/sdcard/SPISDCard.h"
#elif defined(BOARD_WAVESHARE_S3_LCD_7)
#include <Wire.h>
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
  // CH422G must be initialized first — it controls reset lines for LCD and touch.
  // Pin states are written before output mode is enabled to avoid glitching USB_SEL.
  Wire.begin(TOUCH_SDA, TOUCH_SCL);
  CH422G::init();
  CH422G::setPin(CH422G_EXIO_LCD_BL, false);

  // GT911 reset: drive INT LOW during reset to select I2C address 0x5D
  pinMode(TOUCH_INT, OUTPUT);
  digitalWrite(TOUCH_INT, LOW);
  CH422G::setPin(CH422G_EXIO_TP_RST, false);
  delay(10);
  CH422G::setPin(CH422G_EXIO_TP_RST, true);
  delay(50);
  pinMode(TOUCH_INT, INPUT);
  delay(50);

  CH422G::setPin(CH422G_EXIO_LCD_BL, true);
#endif

  _display->init();
  _storage->init();

#ifdef BOARD_WAVESHARE_S3_LCD_7
  // Re-initialize I2C after RGB panel init — the panel driver invalidates
  // the bus state, causing ESP_ERR_INVALID_STATE on subsequent I2C ops.
  Wire.end();
  Wire.begin(TOUCH_SDA, TOUCH_SCL);
#endif

  _touch->init();

  lv_init();
#ifndef BOARD_SIMULATOR
  lv_tick_set_cb((lv_tick_get_cb_t)millis);
#endif

  _display->initLVGL();
}

IDisplay &Device::display() { return *_display; }
ITouch &Device::touchscreen() { return *_touch; }
IStorage &Device::sdcard() { return *_storage; }
