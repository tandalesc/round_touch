#include "src/application/Application.h"
#include "src/device/Device.h"

Device device;
Application app(&device);

void setup(void) {
  Serial.begin(115200);
  device.init();
}

void loop() {
  // TODO render metrics
  app.render();
}