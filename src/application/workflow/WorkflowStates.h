#include "src/application/Application.h"

void NotStartedState(Application *app) {
  Serial.println("Device not started.");
  delay(10000);
}

void ErrorState(Application *app) {
  Device *device = app->device();
  TouchScreen &touch = device->touchscreen();
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      app->ready();
    }
  }
}

// READY INFO1 INFO2 INFO3
// ECOMODE

void EcoModeState(Application *app) {
  Device *device = app->device();
  TouchScreen &touch = device->touchscreen();
  if (touch.available() && touch.gesture() == "SWIPE UP") {
    app->ready();
  }
}

void ReadyState(Application *app) {
  Device *device = app->device();
  TouchScreen &touch = device->touchscreen();
  Workflow &workflow = app->workflow();
  auto gfx = device->display().gfx;
  if (touch.available()) {
    if (touch.gesture() == "SINGLE CLICK") {
      workflow.navigate(DETAILS);
      gfx->fillScreen(BLACK);
      device->showMessage("2002 MAZDA MIATA NB");
    }

    if (touch.gesture() == "SWIPE DOWN") {
      workflow.navigate(ECOMODE);
      gfx->fillScreen(GREEN);
      device->showMessage("ECO MODE");
    }

    if (touch.gesture() == "SWIPE LEFT") {
      workflow.navigate(INFO1);
      gfx->fillScreen(BLACK);
      device->showMessage("GAUGES");
    }
  }
}

void Info1State(Application *app) {
  Device *device = app->device();
  TouchScreen &touch = device->touchscreen();
  Workflow &workflow = app->workflow();
  auto gfx = device->display().gfx;
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      workflow.navigate(ECOMODE);
      gfx->fillScreen(BLACK);
      device->showMessage("ECO MODE");
    }

    if (touch.gesture() == "SWIPE LEFT") {
      workflow.navigate(INFO2);
      gfx->fillScreen(BLACK);
      device->showMessage("TUNING");
    }

    if (touch.gesture() == "SWIPE RIGHT") {
      app->ready();
    }
  }
}

void Info2State(Application *app) {
  Device *device = app->device();
  TouchScreen &touch = device->touchscreen();
  Workflow &workflow = app->workflow();
  auto gfx = device->display().gfx;
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      workflow.navigate(ECOMODE);
      gfx->fillScreen(BLACK);
      device->showMessage("ECO MODE");
    }

    if (touch.gesture() == "SWIPE LEFT") {
      workflow.navigate(INFO3);
      gfx->fillScreen(BLACK);
      device->showMessage("STATISTICS");
    }

    if (touch.gesture() == "SWIPE RIGHT") {
      workflow.navigate(INFO1);
      gfx->fillScreen(BLACK);
      device->showMessage("GAUGES");
    }
  }
}

void Info3State(Application *app) {
  Device *device = app->device();
  TouchScreen &touch = device->touchscreen();
  Workflow &workflow = app->workflow();
  auto gfx = device->display().gfx;
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      workflow.navigate(ECOMODE);
      gfx->fillScreen(BLACK);
      device->showMessage("ECO MODE");
    }

    if (touch.gesture() == "SWIPE RIGHT") {
      workflow.navigate(INFO2);
      gfx->fillScreen(BLACK);
      device->showMessage("TUNING");
    }
  }
}

void DetailsState(Application *app) {
  Device *device = app->device();
  TouchScreen &touch = device->touchscreen();
  Workflow &workflow = app->workflow();
  auto gfx = device->display().gfx;
  if (touch.available()) {
    if (touch.gesture() == "SWIPE DOWN") {
      app->ready();
    }
  }
}

void renderWorkflowState(Application *app) {
  State state = app->workflow().getState();
  if (state == NOT_STARTED) {
    NotStartedState(app);
  } else if (state == ERROR) {
    ErrorState(app);
  } else if (state == READY) {
    ReadyState(app);
  } else if (state == ECOMODE) {
    EcoModeState(app);
  } else if (state == INFO1) {
    Info1State(app);
  } else if (state == INFO2) {
    Info2State(app);
  } else if (state == INFO3) {
    Info3State(app);
  } else if (state == DETAILS) {
    DetailsState(app);
  }
}