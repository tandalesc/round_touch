#include "src/device/Device.h"

Device device;

void setup(void) {
  Serial.begin(115200);
  device.init();
}

void loop() {
  switch (device.workflow.getState()) {
  case NOT_STARTED:
    return NotStartedState();
  case ERROR:
    return ErrorState();
  case READY:
  default:
    return ReadyState();
  }
}

void NotStartedState() {
  Serial.println("Device not started.");
  delay(3000);
}

void ErrorState() {
  TouchScreen &touch = device.touchscreen();
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      device.ready();
    }
  }
}

void ReadyState() {
  TouchScreen &touch = device.touchscreen();
  if (touch.available()) {
    if (touch.gesture() == "SWIPE UP") {
      device.error("MENU");
    }
  }
}