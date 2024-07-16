#ifndef _TOUCH_EVENT_T_
#define _TOUCH_EVENT_T_

#include "src/device/types/TouchLocation.h"

struct TouchEvent {
  enum Type { Swipe, Tap, Unknown } type;
  enum Direction { None, Up, Down, Left, Right } direction = None;
  TouchLocation location = {};
  unsigned long timestamp = 0;
};

static void printTouchEvent(TouchEvent &event) {
  if (event.type == TouchEvent::Tap) {
    Serial.print("Received Tap at ");
    Serial.print(event.location.x);
    Serial.print(", ");
    Serial.print(event.location.y);
    Serial.println();
  } else if (event.type == TouchEvent::Swipe) {
    Serial.print("Received a swipe ");
    if (event.direction == TouchEvent::Up) {
      Serial.print("up");
    } else if (event.direction == TouchEvent::Down) {
      Serial.print("down");
    } else if (event.direction == TouchEvent::Left) {
      Serial.print("left");
    } else if (event.direction == TouchEvent::Right) {
      Serial.print("right");
    }
    Serial.println();
  }
}

#endif // _TOUCH_EVENT_T_