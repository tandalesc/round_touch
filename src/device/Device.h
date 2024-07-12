#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "src/config/Constants.h"

#include "src/device/hw/Display.h"
#include "src/device/hw/SDCard.h"
#include "src/device/hw/Touch.h"

#include "src/lib/jpeg/JpegFunctions.h"
#include "src/workflow/Workflow.h"

class Device {
private:
  void showSplashScreen();
  Workflow _workflow;

public:
  void init();
  void ready();
  void showMessage(const char *msg);

  Display &display();
  TouchScreen &touchscreen();
  SDCard &sdcard();
  Workflow &workflow();
};

#endif // _DEVICE_H_