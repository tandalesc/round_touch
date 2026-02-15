#include "lvgl.h"

#include "application/Application.h"

#include "application/interface/Interface.h"
#include "application/interface/components/ComponentManager.h"
#include "application/interface/components/types/Component.h"

#include "events/types/TouchEvent.h"

Interface::Interface(Application *app) {
  this->app = app;
  manager = new ComponentManager(app);
}

// this automatically deletes the components contained within
Interface::~Interface() { delete manager; }

void Interface::loop() {
  if (refresh) {
    manager->createComponent(app->workflow().getState());
    refresh = false;
  }
  lv_timer_handler();
}

void Interface::handleEvent(InputEvent &event) {
  // manager needs to dispatch this event to the active components
  manager->handleEvent(event);
}

void Interface::handleEvent(WorkflowEvent &event) {
  // there is only one kind of workflow event right now, so
  // regardless of what kind it is, trigger a refresh.
  refresh = true;
}