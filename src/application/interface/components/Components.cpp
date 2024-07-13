#include "src/application/interface/components/Components.h"

RenderableComponent DeviceNotStarted() {
  Serial.println("Device not started.");
  delay(10000);
  return E(EmptyComponent);
}

RenderableComponent ErrorState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available() && touch.gesture() == "SWIPE DOWN") {
    workflow.navigate(READY);
  }
  return E(FillScreen, {BLACK}, E(Text, {"Error!"}));
}

// READY INFO1 INFO2 INFO3
// ECOMODE

RenderableComponent EcoModeState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available() && touch.gesture() == "SWIPE DOWN") {
    workflow.navigate(READY);
  }
  return E(FillScreen, {.color = GREEN},
           E(Text, {.text = "ECO Mode", .color = BLACK, .size = 4}));
}

RenderableComponent ReadyState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available()) {
    if (touch.gesture() == "SINGLE CLICK") {
      workflow.navigate(DETAILS);
    } else if (touch.gesture() == "SWIPE UP") {
      workflow.navigate(ECOMODE);
    } else if (touch.gesture() == "SWIPE LEFT") {
      workflow.navigate(INFO1);
    }
  }
  return E(FillScreen, {BLACK}, E(Text, {"Details"}));
}

RenderableComponent Info1State(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available()) {
    if (touch.gesture() == "SWIPE UP") {
      workflow.navigate(ECOMODE);
    } else if (touch.gesture() == "SWIPE LEFT") {
      workflow.navigate(INFO2);
    } else if (touch.gesture() == "SWIPE RIGHT") {
      workflow.navigate(READY);
    }
  }
  return E(FillScreen, {BLACK}, E(Text, {"Gauges"}));
}

RenderableComponent Info2State(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available()) {
    if (touch.gesture() == "SWIPE UP") {
      workflow.navigate(ECOMODE);
    } else if (touch.gesture() == "SWIPE LEFT") {
      workflow.navigate(INFO3);
    } else if (touch.gesture() == "SWIPE RIGHT") {
      workflow.navigate(INFO1);
    }
  }
  return E(FillScreen, {BLACK}, E(Text, {"Tuning"}));
}

RenderableComponent Info3State(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available()) {
    if (touch.gesture() == "SWIPE UP") {
      workflow.navigate(ECOMODE);
    } else if (touch.gesture() == "SWIPE RIGHT") {
      workflow.navigate(INFO2);
    }
  }
  return new FillScreen({BLACK}, new Text({"Settings"}));
}

RenderableComponent DetailsState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available() && touch.gesture() == "SWIPE DOWN") {
    workflow.navigate(READY);
  }
  return E(FillScreen, {.color = BLACK},
           E(Text, {.text = "2002 Mazda Miata NB", .size = 2}));
}