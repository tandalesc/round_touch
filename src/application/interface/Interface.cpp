#include "src/application/Application.h"

#include "src/application/interface/Interface.h"
#include "src/application/interface/components/Components.h"

RenderableComponent createComponent(Application *app, State state) {
  if (state == NOT_STARTED) {
    return DeviceNotStarted();
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

void Interface::process(Application *app) {
  bool render = false;
  // if workflow changed, trigger render
  Workflow &workflow = app->workflow();
  if (workflow.hasChanges()) {
    Serial.println("Something changed, re-rendering.");
    workflow.applyChanges();
    render = true;
  }
  // renderable component must always be deleted after using
  RenderableComponent component = createComponent(app, workflow.getState());
  if (render) {
    LayoutContext rootLayout = {.position = {.t = 80, .l = 10}};
    component->calculateSize();
    component->updateLayout(rootLayout);
    component->render(app);
  }
  delete component;
}