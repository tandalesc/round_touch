#include "src/application/interface/components/Components.h"

RenderableComponent DeviceNotStarted() {
  Serial.println("Device not started.");
  delay(10000);
  return E(Component);
}

RenderableComponent ErrorState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available() && touch.gesture() == "SWIPE DOWN") {
    workflow.navigate(READY);
  }
  return (
    E(FillScreen, {BLACK},
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Error!")
      )
    )
  );
}

// READY INFO1 INFO2 INFO3
// ECOMODE

RenderableComponent EcoModeState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available() && touch.gesture() == "SWIPE DOWN") {
    workflow.navigate(READY);
  }
  return (
    E(FillScreen, {.color = GREEN},
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, {.color = BLACK}, "ECO Mode")
      )
    )
  );
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
  return (
    E(FillScreen, {BLACK},
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Details")
      )
    )
  );
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
  return (
    E(FillScreen, {BLACK},
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Gauges")
      )
    )
  );
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
  return (
    E(FillScreen, {BLACK},
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Tuning")
      )
    )
  );
}

RenderableComponent Info3State(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available()) {
    if (touch.gesture() == "SWIPE UP") {
      workflow.navigate(ECOMODE);
    } else if (touch.gesture() == "SWIPE RIGHT") {
      workflow.navigate(INFO2);
    } else if (touch.gesture() == "SINGLE CLICK") {
      app->events().addEvent({EventType::TouchEvent});
    }
  }
  return (
    E(FillScreen, {BLACK},
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Settings"),
        E(SwipeInput)
      )
    )
  );
}

RenderableComponent DetailsState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available() && touch.gesture() == "SWIPE DOWN") {
    workflow.navigate(READY);
  }
  return (
    E(FillScreen, {BLACK},
      E(FlexLayout, {.type = LayoutType::Column},
        E(FlexLayout, {.type = LayoutType::Row, .padding = {.l = 5}},
          E(FlexLayout, {.type = LayoutType::Column, .padding = {.t = 5}},
            E(Text, {.size = 3}, "2002")
          ),
          E(FlexLayout, {.type = LayoutType::Column, .padding = {.l = 5}},
            E(Text, {.size = 2}, "Mazda Miata"),
            E(Text, {.size = 2}, "1.8L Stock")
          )
        ),
        E(FlexLayout, {.type = LayoutType::Column, .padding = {.t = 5, .l = 12}},
          E(Text, {.size = 2}, "Special Edition"),
          E(Text, {.size = 2}, "6 Speed")
        )
      )
    )
  );
}