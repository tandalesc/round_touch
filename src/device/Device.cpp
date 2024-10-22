#include "src/device/Device.h"

void Device::init() {
  this->display();
  this->sdcard();
  this->touchscreen();
  delay(1000);
  if (this->sdcard().errorWhileInit) {
    Serial.println("SD Card initialization error");
    return;
  }
  this->showSplashScreen();
}

void Device::showMessage(const char *msg) {
  auto gfx = this->display().gfx;
  gfx->setTextSize(4);
  gfx->setCursor(10, 80);
  gfx->println(F(msg));
}

void Device::showSplashScreen() {
  drawJpegFullscreen(SPLASH_SCREEN_JPEG_PATH);
  delay(2000);
}

// Hardware
Display &Device::display() { return Display::getInstance(); }
TouchScreen &Device::touchscreen() { return TouchScreen::getInstance(); }
SDCard &Device::sdcard() { return SDCard::getInstance(); }
