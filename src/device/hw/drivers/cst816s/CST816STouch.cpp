#include "device/hw/drivers/cst816s/CST816STouch.h"
#include "BoardConfig.h"
#include "lvgl.h"

static TouchLocation flipTouch(TouchLocation loc) {
  if (lv_display_get_rotation(NULL) == LV_DISPLAY_ROTATION_180) {
    loc.x = SCREEN_WIDTH - 1 - loc.x;
    loc.y = SCREEN_HEIGHT - 1 - loc.y;
  }
  return loc;
}

static SwipeDirection flipSwipe(SwipeDirection dir) {
  if (lv_display_get_rotation(NULL) != LV_DISPLAY_ROTATION_180) return dir;
  switch (dir) {
    case SwipeDirection::SwipeUp: return SwipeDirection::SwipeDown;
    case SwipeDirection::SwipeDown: return SwipeDirection::SwipeUp;
    case SwipeDirection::SwipeLeft: return SwipeDirection::SwipeRight;
    case SwipeDirection::SwipeRight: return SwipeDirection::SwipeLeft;
    default: return dir;
  }
}

void CST816STouch::pollEvent(EventHandler<InputEvent> *handler) {
  if (!available()) {
    return;
  }
  String gesture = this->gesture();
  if (gesture == "SINGLE CLICK") {
    TapTouchEvent event = TapTouchEvent(flipTouch(location()));
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
    direction = flipSwipe(direction);
    SwipeTouchEvent event = SwipeTouchEvent(direction, flipTouch(location()));
    handler->handleEvent(event);
    return;
  }
}
