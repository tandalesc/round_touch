#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <CST816S.h>

#include "src/device/types/TouchLocation.h"

#define TOUCH_SDA 38
#define TOUCH_SCL 39
#define TOUCH_RST 16
#define TOUCH_INT 40

class TouchScreen {
private:
  CST816S *driver;
  TouchScreen() {
    driver = new CST816S(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
    driver->begin();
  }

public:
  static TouchScreen &getInstance() {
    static TouchScreen ts;
    return ts;
  }
  ~TouchScreen() { delete driver; }
  TouchScreen(TouchScreen const &) = delete;
  void operator=(TouchScreen const &) = delete;

  bool available() { return driver->available(); }
  String gesture() { return driver->gesture(); }
  TouchLocation location() {
    TouchLocation location = {driver->data.x, driver->data.y};
    return location;
  }
  void printDebugMessage() {
    if (this->available()) {
      TouchLocation location = this->location();
      Serial.print(this->gesture());
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
};

#endif // _TOUCH_H_