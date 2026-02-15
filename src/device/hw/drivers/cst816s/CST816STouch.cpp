#include "device/hw/drivers/cst816s/CST816STouch.h"

void CST816STouch::pollEvent(EventHandler<InputEvent> *handler) {
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
