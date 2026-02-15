#ifndef _SIM_DRIVERS_H_
#define _SIM_DRIVERS_H_

#include "device/IDisplay.h"
#include "device/ITouch.h"
#include "device/IStorage.h"
#include "device/types/TouchLocation.h"
#include "events/types/TouchEvent.h"
#include "platform/SimTouch.h"

// Simulator display - LVGL handles rendering via its own SDL driver
class SimDisplayDriver : public IDisplay {
public:
  void init() override {}
  int width() override { return SCREEN_WIDTH; }
  int height() override { return SCREEN_HEIGHT; }
};

// Simulator touch wraps SimTouch
class SimTouchDriver : public ITouch {
public:
  void init() override {}

  void pollEvent(EventHandler<InputEvent> *handler) override {
    if (!SimTouch::hasPendingGesture()) return;

    auto pos = SimTouch::getGesturePos();
    std::string gestureStr = SimTouch::consumeGesture();
    String gesture(gestureStr.c_str());

    if (gesture == "SINGLE CLICK") {
      TouchLocation loc = {pos.first, pos.second};
      TapTouchEvent event = TapTouchEvent(loc);
      handler->handleEvent(event);
      return;
    }
    if (gesture == "SWIPE UP" || gesture == "SWIPE DOWN" ||
        gesture == "SWIPE LEFT" || gesture == "SWIPE RIGHT") {
      SwipeDirection direction = SwipeDirection::UnknownDirection;
      if (gesture == "SWIPE UP") direction = SwipeDirection::SwipeDown;
      else if (gesture == "SWIPE DOWN") direction = SwipeDirection::SwipeUp;
      else if (gesture == "SWIPE LEFT") direction = SwipeDirection::SwipeLeft;
      else if (gesture == "SWIPE RIGHT") direction = SwipeDirection::SwipeRight;
      SwipeTouchEvent event = SwipeTouchEvent(direction);
      handler->handleEvent(event);
      return;
    }
  }
};

// Simulator storage is a no-op (no SD card)
class SimStorageDriver : public IStorage {
public:
  void init() override {}
  bool hasError() override { return true; } // no SD in simulator
  File open_r(const char *) override { return File(); }
};

#endif // _SIM_DRIVERS_H_
