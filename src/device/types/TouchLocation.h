#ifndef _TOUCH_LOCATION_H_
#define _TOUCH_LOCATION_H_

struct TouchLocation {
  int x = 0, y = 0;
};

struct TouchRegion {
  TouchLocation start = {}, end = {1000, 1000};

  bool contains(TouchLocation point) {
    return (start.x < point.x && point.x < end.x && start.y < point.y &&
            point.y < end.y);
  }
};

#endif // _TOUCH_LOCATION_H_