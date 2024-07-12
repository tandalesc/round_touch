#include "src/application/Application.h"

RenderFunction ErrorState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available() && touch.gesture() == "SWIPE DOWN") {
    workflow.navigate(READY);
  }
  return [](Application *app) -> void {
    auto gfx = app->device()->display().gfx;
    gfx->fillScreen(BLACK);
    app->device()->showMessage("Error!");
  };
}

// READY INFO1 INFO2 INFO3
// ECOMODE

RenderFunction EcoModeState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available() && touch.gesture() == "SWIPE DOWN") {
    workflow.navigate(READY);
  }
  return [](Application *app) -> void {
    auto gfx = app->device()->display().gfx;
    gfx->fillScreen(GREEN);
    app->device()->showMessage("ECO Mode");
  };
}

RenderFunction ReadyState(Application *app) {
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
  return [](Application *app) -> void {
    auto gfx = app->device()->display().gfx;
    gfx->fillScreen(BLACK);
    app->device()->showMessage("Details");
  };
}

RenderFunction Info1State(Application *app) {
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
  return [](Application *app) -> void {
    auto gfx = app->device()->display().gfx;
    gfx->fillScreen(BLACK);
    app->device()->showMessage("Gauges");
  };
}

RenderFunction Info2State(Application *app) {
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
  return [](Application *app) -> void {
    auto gfx = app->device()->display().gfx;
    gfx->fillScreen(BLACK);
    app->device()->showMessage("Tuning");
  };
}

RenderFunction Info3State(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available()) {
    if (touch.gesture() == "SWIPE UP") {
      workflow.navigate(ECOMODE);
    } else if (touch.gesture() == "SWIPE RIGHT") {
      workflow.navigate(INFO2);
    }
  }
  return [](Application *app) -> void {
    auto gfx = app->device()->display().gfx;
    gfx->fillScreen(BLACK);
    app->device()->showMessage("Settings");
  };
}

RenderFunction DetailsState(Application *app) {
  TouchScreen &touch = app->device()->touchscreen();
  Workflow &workflow = app->workflow();
  if (touch.available() && touch.gesture() == "SWIPE DOWN") {
    workflow.navigate(READY);
  }
  return [](Application *app) -> void {
    auto gfx = app->device()->display().gfx;
    gfx->fillScreen(BLACK);
    app->device()->showMessage("2002 Mazda Miata NB");
  };
}

RenderFunction SkipRender = [](Application *app) -> void {};

RenderFunction processState(Application *app, State state) {
  if (state == NOT_STARTED) {
    Serial.println("Device not started.");
    delay(10000);
    return SkipRender;
  } else if (state == ERROR) {
    return ErrorState(app);
  } else if (state == READY) {
    return ReadyState(app);
  } else if (state == ECOMODE) {
    return EcoModeState(app);
  } else if (state == INFO1) {
    return Info1State(app);
  } else if (state == INFO2) {
    return Info2State(app);
  } else if (state == INFO3) {
    return Info3State(app);
  } else if (state == DETAILS) {
    return DetailsState(app);
  }
}