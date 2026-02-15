#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "config/Constants.h"

#include "device/hw/Display.h"
#include "device/hw/SDCard.h"
#include "device/hw/Touch.h"

#include "lib/jpeg/JpegFunctions.h"

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