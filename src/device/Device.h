#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "src/config/Constants.h"

#include "src/device/hw/Display.h"
#include "src/device/hw/SDCard.h"
#include "src/device/hw/Touch.h"

#include "src/lib/jpeg/JpegFunctions.h"

class Device {
private:
  void showSplashScreen();

public:
  void init();
  void showMessage(const char *msg);

  Display &display();
  TouchScreen &touchscreen();
  SDCard &sdcard();
};

#endif // _DEVICE_H_