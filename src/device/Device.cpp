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

void Device::pollEvent(EventHandler<TouchEvent> &handler) {
  TouchScreen &touch = touchscreen();
  if (!touch.available()) {
    return;
  }
  TouchEvent event = {.type = TouchEvent::Unknown, .timestamp = millis()};
  String gesture = touch.gesture();
  if (gesture == "SINGLE CLICK") {
    event.type = TouchEvent::Tap;
    event.location = touch.location();
  } else if (gesture == "SWIPE UP" || gesture == "SWIPE DOWN" ||
             gesture == "SWIPE LEFT" || gesture == "SWIPE RIGHT") {
    event.type = TouchEvent::Swipe;
    if (gesture == "SWIPE UP") {
      // swap down and up to match hardware
      event.direction = TouchEvent::Down;
    } else if (gesture == "SWIPE DOWN") {
      event.direction = TouchEvent::Up;
    } else if (gesture == "SWIPE LEFT") {
      event.direction = TouchEvent::Left;
    } else if (gesture == "SWIPE RIGHT") {
      event.direction = TouchEvent::Right;
    }
  }
  handler.handleEvent(event);
}

// Hardware
Display &Device::display() { return Display::getInstance(); }
TouchScreen &Device::touchscreen() { return TouchScreen::getInstance(); }
SDCard &Device::sdcard() { return SDCard::getInstance(); }
