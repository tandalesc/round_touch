#ifndef _CST816S_SHIM_H_
#define _CST816S_SHIM_H_

#include "Arduino.h"
#include "platform/SimTouch.h"

class CST816S {
public:
  struct {
    int x = 0;
    int y = 0;
  } data;

  CST816S(int, int, int, int) {}

  void begin() {}

  bool available() {
    if (SimTouch::hasPendingGesture()) {
      auto pos = SimTouch::getGesturePos();
      data.x = pos.first;
      data.y = pos.second;
      return true;
    }
    return false;
  }

  String gesture() {
    return String(SimTouch::consumeGesture().c_str());
  }
};

#endif // _CST816S_SHIM_H_
