#ifndef _CST816S_TOUCH_H_
#define _CST816S_TOUCH_H_

#include <CST816S.h>

#include "device/ITouch.h"
#include "device/types/TouchLocation.h"
#include "events/types/TouchEvent.h"

class CST816STouch : public ITouch {
private:
  CST816S *driver = nullptr;

public:
  CST816STouch() {}
  ~CST816STouch() { delete driver; }

  void init() override {
    driver = new CST816S(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
    driver->begin();
  }

  void pollEvent(EventHandler<InputEvent> *handler) override;

private:
  bool available() { return driver->available(); }
  String gesture() { return driver->gesture(); }
  TouchLocation location() {
    return {driver->data.x, driver->data.y};
  }
};

#endif // _CST816S_TOUCH_H_
