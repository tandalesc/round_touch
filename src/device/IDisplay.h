#ifndef _IDISPLAY_H_
#define _IDISPLAY_H_

#include "lvgl.h"

class IDisplay {
public:
  virtual ~IDisplay() {}
  virtual void init() = 0;
  virtual int width() = 0;
  virtual int height() = 0;
  virtual bool isCircular() { return false; }

  // Create and configure the LVGL display driver for this hardware.
  // Called after init() and lv_init(). Returns the lv_display handle.
  virtual lv_display_t *initLVGL() = 0;
};

#endif // _IDISPLAY_H_
