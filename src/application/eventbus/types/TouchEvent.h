#ifndef _TOUCH_EVENT_T_
#define _TOUCH_EVENT_T_

#include "src/device/types/TouchLocation.h"

struct TouchEvent {
  enum { Swipe, Tap } type;
  enum { None, Up, Down, Left, Right } direction = None;
  TouchLocation location = {};
  unsigned long timestamp = 0;
};

#endif // _TOUCH_EVENT_T_