#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <CST816S.h>

#include "device/types/TouchLocation.h"
#include "events/EventSource.h"
#include "events/types/TouchEvent.h"

#define TOUCH_SDA 38
#define TOUCH_SCL 39
#define TOUCH_RST 16
#define TOUCH_INT 40

class TouchScreen : public EventSource<InputEvent> {
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

  bool available();
  String gesture();
  TouchLocation location();
  void printDebugMessage();

  void pollEvent(EventHandler<InputEvent> *handler);
};

#endif // _TOUCH_H_