#include "src/device/Device.h"

#include "src/application/Application.h"

Device device;
Application app(&device);

void setup(void) {
  Serial.begin(115200);
  device.init();
  app.init();
}

void loop() {
  // TODO render metrics
  app.loop();
}