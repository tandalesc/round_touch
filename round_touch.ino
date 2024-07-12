#include "src/device/Device.h"

Device device;

void setup(void) {
  Serial.begin(115200);
  device.init();
}

void loop() {
  State state = device.workflow().getState();
  if (state == NOT_STARTED) {
    NotStartedState();
  } else if (state == ERROR) {
    ErrorState();
  } else if (state == READY) {
    ReadyState();
  } else if (state == ECOMODE) {
    EcoModeState();
  } else if (state == INFO1) {
    Info1State();
  } else if (state == INFO2) {
    Info2State();
  } else if (state == INFO3) {
    Info3State();
  }
  delay(500);
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
      delay(500);
    }
  }
}

// ECOMODE
// READY INFO1 INFO2 INFO3

void EcoModeState() {
  TouchScreen &touch = device.touchscreen();
  if (touch.available() && touch.gesture() == "SWIPE UP") {
    device.ready();
  }
}

void ReadyState() {
  TouchScreen &touch = device.touchscreen();
  Workflow &workflow = device.workflow();
  auto gfx = device.display().gfx;
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      workflow.navigate(ECOMODE);
      gfx->fillScreen(BLACK);
      device.showMessage("ECO MODE");
    }

    if (touch.gesture() == "SWIPE LEFT") {
      workflow.navigate(INFO1);
      gfx->fillScreen(BLACK);
      device.showMessage("GAUGES");
    }
  }
}

void Info1State() {
  TouchScreen &touch = device.touchscreen();
  Workflow &workflow = device.workflow();
  auto gfx = device.display().gfx;
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      workflow.navigate(ECOMODE);
      gfx->fillScreen(BLACK);
      device.showMessage("ECO MODE");
    }

    if (touch.gesture() == "SWIPE LEFT") {
      workflow.navigate(INFO2);
      gfx->fillScreen(BLACK);
      device.showMessage("TUNING");
    }

    if (touch.gesture() == "SWIPE RIGHT") {
      device.ready();
    }
  }
}

void Info2State() {
  TouchScreen &touch = device.touchscreen();
  Workflow &workflow = device.workflow();
  auto gfx = device.display().gfx;
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      workflow.navigate(ECOMODE);
      gfx->fillScreen(BLACK);
      device.showMessage("ECO MODE");
    }

    if (touch.gesture() == "SWIPE LEFT") {
      workflow.navigate(INFO3);
      gfx->fillScreen(BLACK);
      device.showMessage("STATISTICS");
    }

    if (touch.gesture() == "SWIPE RIGHT") {
      workflow.navigate(INFO1);
      gfx->fillScreen(BLACK);
      device.showMessage("GAUGES");
    }
  }
}

void Info3State() {
  TouchScreen &touch = device.touchscreen();
  Workflow &workflow = device.workflow();
  auto gfx = device.display().gfx;
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      workflow.navigate(ECOMODE);
      gfx->fillScreen(BLACK);
      device.showMessage("ECO MODE");
    }

    if (touch.gesture() == "SWIPE RIGHT") {
      workflow.navigate(INFO2);
      gfx->fillScreen(BLACK);
      device.showMessage("TUNING");
    }
  }
}