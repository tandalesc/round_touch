#include "device/hw/drivers/gt911/GT911Touch.h"

#include <cmath>

// Swipe detection thresholds
#define SWIPE_MIN_DISTANCE 50

void GT911Touch::pollEvent(EventHandler<InputEvent> *handler) {
  uint8_t status = readReg(GT911_REG_STATUS);
  uint8_t touchCount = status & 0x0F;
  bool bufferReady = status & 0x80;

  if (!bufferReady) return;

  // Clear status register
  writeReg(GT911_REG_STATUS, 0);

  if (touchCount > 0) {
    // Read first touch point (8 bytes per point)
    uint8_t buf[8];
    readRegs(GT911_REG_POINT1, buf, 8);

    uint16_t x = buf[1] | (buf[2] << 8);
    uint16_t y = buf[3] | (buf[4] << 8);

    _lastPos = {(int)x, (int)y};
    if (!_wasTouched) {
      _wasTouched = true;
      _touchStart = _lastPos;
    }
  } else if (_wasTouched) {
    _wasTouched = false;

    int dx = _lastPos.x - _touchStart.x;
    int dy = _lastPos.y - _touchStart.y;
    int dist = (int)sqrt(dx * dx + dy * dy);

    if (dist < SWIPE_MIN_DISTANCE) {
      TapTouchEvent event = TapTouchEvent(_touchStart);
      handler->handleEvent(event);
    } else {
      SwipeDirection dir;
      if (abs(dx) > abs(dy)) {
        dir = dx > 0 ? SwipeDirection::SwipeRight : SwipeDirection::SwipeLeft;
      } else {
        dir = dy > 0 ? SwipeDirection::SwipeDown : SwipeDirection::SwipeUp;
      }
      SwipeTouchEvent event = SwipeTouchEvent(dir);
      handler->handleEvent(event);
    }
  }
}
