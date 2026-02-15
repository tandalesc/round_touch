#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "config/Constants.h"

#include "device/IDisplay.h"
#include "device/ITouch.h"
#include "device/IStorage.h"

class Device {
private:
  IDisplay *_display = nullptr;
  ITouch *_touch = nullptr;
  IStorage *_storage = nullptr;

public:
  Device();
  ~Device();

  void init();

  IDisplay &display();
  ITouch &touchscreen();
  IStorage &sdcard();
};

#endif // _DEVICE_H_
