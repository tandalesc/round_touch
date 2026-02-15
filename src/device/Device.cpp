#include "device/Device.h"

#include "lib/jpeg/JpegFunctions.h"

#ifdef BOARD_MAKERFABS_ROUND_128
#include "device/hw/drivers/gc9a01/GC9A01Display.h"
#include "device/hw/drivers/cst816s/CST816STouch.h"
#include "device/hw/drivers/sdcard/SPISDCard.h"
#elif defined(BOARD_SIMULATOR)
#include "SimDrivers.h"
#endif

Device::Device() {
#ifdef BOARD_MAKERFABS_ROUND_128
  _display = new GC9A01Display();
  _touch = new CST816STouch();
  _storage = new SPISDCard();
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
  _display->init();
  _storage->init();
  _touch->init();
  delay(1000);
  if (_storage->hasError()) {
    Serial.println("SD Card initialization error");
    return;
  }
  showSplashScreen();
}

void Device::showMessage(const char *msg) {
  auto gfx = _display->gfx();
  gfx->setTextSize(4);
  gfx->setCursor(10, 80);
  gfx->println(F(msg));
}

void Device::showSplashScreen() {
  drawJpegFullscreen(*_display, *_storage, SPLASH_SCREEN_JPEG_PATH);
  delay(2000);
}

IDisplay &Device::display() { return *_display; }
ITouch &Device::touchscreen() { return *_touch; }
IStorage &Device::sdcard() { return *_storage; }
