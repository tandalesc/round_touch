#include "Touch.h"

bool TouchScreen::available() { return driver->available(); }

String TouchScreen::gesture() { return driver->gesture(); }

TouchLocation TouchScreen::location() {
  TouchLocation location = {driver->data.x, driver->data.y};
  return location;
}

void TouchScreen::printDebugMessage() {
  if (available()) {
    TouchLocation location = this->location();
    Serial.print(gesture());
    // Serial.print("\t");
    // Serial.print(touch.data.points);
    // Serial.print("\t");
    // Serial.print(touch.data.event);
    Serial.print("\t");
    Serial.print(location.x);
    Serial.print("\t");
    Serial.println(location.y);
  }
}

void TouchScreen::pollEvent(EventHandler<InputEvent> *handler) {
  if (!available()) {
    return;
  }
  String gesture = this->gesture();
  if (gesture == "SINGLE CLICK") {
    TapTouchEvent event = TapTouchEvent(location());
    handler->handleEvent(event);
    return;
  }
  if (gesture == "SWIPE UP" || gesture == "SWIPE DOWN" ||
      gesture == "SWIPE LEFT" || gesture == "SWIPE RIGHT") {
    SwipeDirection direction = SwipeDirection::UnknownDirection;
    // swap down and up to match hardware
    if (gesture == "SWIPE UP") {
      direction = SwipeDirection::SwipeDown;
    } else if (gesture == "SWIPE DOWN") {
      direction = SwipeDirection::SwipeUp;
    } else if (gesture == "SWIPE LEFT") {
      direction = SwipeDirection::SwipeLeft;
    } else if (gesture == "SWIPE RIGHT") {
      direction = SwipeDirection::SwipeRight;
    }
    SwipeTouchEvent event = SwipeTouchEvent(direction);
    handler->handleEvent(event);
    return;
  }
}