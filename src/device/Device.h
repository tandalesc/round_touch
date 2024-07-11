#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "src/device/hw/Display.h"
#include "src/device/hw/SDCard.h"
#include "src/device/hw/Touch.h"

#include "src/jpeg/JpegFunctions.h"
#include "src/workflow/Workflow.h"

class Device {
private:
  void showErrorMessage(const char *msg);
  void showSplashScreen();

public:
  Workflow workflow;
  void init();
  void ready();
  void error(const char *msg);

  Display &display();
  TouchScreen &touchscreen();
  SDCard &sdcard();
};

#endif // _DEVICE_H_