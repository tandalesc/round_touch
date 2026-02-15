#include "application/Application.h"

#include "application/interface/components/ComponentManager.h"
#include "application/workflow/ComponentLibrary.h"

void ComponentManager::createComponent(State state) {
  // if we are already assigned to a component, destroy it first
  if (active != nullptr) {
    deleteComponent();
  }

  // create a new LVGL screen
  screen = lv_obj_create(NULL);
  lv_obj_remove_style_all(screen);

  // create the component tree from the declarative DSL
  active = createComponentFromState(state);
  active->attachApplication(app);
  // build the LVGL widget tree on the screen
  active->createWidgets(screen);
  // load the screen (with no animation for now)
  lv_screen_load(screen);
}

void ComponentManager::deleteComponent() {
  if (active != nullptr) {
    delete active;
    active = nullptr;
  }
  // the old screen will be cleaned up by LVGL when replaced
  screen = nullptr;
}

void ComponentManager::handleEvent(InputEvent &event) {
  if (active != nullptr) {
    active->handleEvent(event);
  }
}
