#include "src/config/Constants.h"
#include "src/device/Device.h"

Device device;

struct DeviceContext {
  int page;
} context = {0};

void setup(void) {
  Serial.begin(115200);
  device.init();
}

void loop() {
  switch (device.workflow.getState()) {
  case NOT_STARTED:
    return NotStartedState(&context);
  case ERROR:
    return ErrorState(&context);
  case READY:
  default:
    return ReadyState(&context);
  }
}

void NotStartedState(DeviceContext *ctx) {
  Serial.println("Device not started.");
  delay(3000);
}

void ErrorState(DeviceContext *ctx) {
  TouchScreen &touch = device.touchscreen();
  if (touch.available()) {
    TouchLocation location = touch.location();
    if (touch.gesture() == "SWIPE DOWN") {
      device.ready();
    }
  }
}

void ReadyState(DeviceContext *ctx) {
  TouchScreen &touch = device.touchscreen();
  if (touch.available()) {
    TouchLocation location = touch.location();
    if (touch.gesture() == "SWIPE UP") {
      device.error("MENU");
    }
  }
}