#ifndef _IDISPLAY_H_
#define _IDISPLAY_H_

#include <Arduino_GFX_Library.h>

class IDisplay {
public:
  virtual ~IDisplay() {}
  virtual void init() = 0;
  virtual Arduino_GFX *gfx() = 0;
  virtual int width() = 0;
  virtual int height() = 0;
};

#endif // _IDISPLAY_H_
