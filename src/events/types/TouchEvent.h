#ifndef _TOUCH_EVENT_T_
#define _TOUCH_EVENT_T_

#include "src/device/types/TouchLocation.h"
#include "src/events/types/InputEvent.h"

enum class TouchType { UnknownType, SwipeType, TapType };
enum class SwipeDirection {
  UnknownDirection,
  SwipeUp,
  SwipeDown,
  SwipeLeft,
  SwipeRight
};

class TouchEvent : public InputEvent {
public:
  unsigned long timestamp = 0;
  TouchType type;
  TouchLocation location = {};

  TouchEvent(TouchType type)
      : InputEvent(EventType::UserInputEvent, InputType::TouchInput),
        type(type) {
    timestamp = millis();
  }

  virtual void debug() override {
    Serial.print("TouchEvent: Received ");
    if (type == TouchType::TapType) {
      Serial.print("tap");
    } else if (type == TouchType::SwipeType) {
      Serial.print("swipe");
    } else {
      Serial.print("unknown touch");
    }
    Serial.println("event.");
  }
};

class TapTouchEvent : public TouchEvent {
public:
  TouchLocation location = {};

  TapTouchEvent(TouchLocation location)
      : TouchEvent(TouchType::TapType), location(location) {};

  virtual void debug() override {
    Serial.print("TapTouchEvent: Received tap event at (");
    Serial.print(location.x);
    Serial.print(", ");
    Serial.print(location.y);
    Serial.println(").");
  }
};

class SwipeTouchEvent : public TouchEvent {
public:
  SwipeDirection direction;

  SwipeTouchEvent(SwipeDirection direction)
      : TouchEvent(TouchType::SwipeType), direction(direction) {};

  virtual void debug() override {
    Serial.print("SwipeTouchEvent: Received swipe ");
    if (direction == SwipeDirection::SwipeUp) {
      Serial.print("up");
    } else if (direction == SwipeDirection::SwipeDown) {
      Serial.print("down ");
    } else if (direction == SwipeDirection::SwipeLeft) {
      Serial.print("left ");
    } else if (direction == SwipeDirection::SwipeRight) {
      Serial.print("right ");
    }
    Serial.println("event.");
  }
};

#endif // _TOUCH_EVENT_T_