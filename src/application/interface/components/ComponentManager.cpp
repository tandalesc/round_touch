#include "src/application/Application.h"

#include "src/application/interface/components/ComponentManager.h"
#include "src/application/workflow/ComponentLibrary.h"

void ComponentManager::createComponent(State state) {
  // if we are already assigned to a component, destroy it first
  if (active != nullptr) {
    deleteComponent();
  }
  active = createComponentFromState(state);
}

void ComponentManager::renderComponent() {
  if (active != nullptr) {
    active->attachApplication(app);
    active->calculateSize();
    active->updateLayout(rootLayout);
    active->render(app);
  }
}

void ComponentManager::deleteComponent() {
  if (active != nullptr) {
    delete active;
    active = nullptr;
  }
}

void ComponentManager::handleEvent(TouchEvent &event) {
  if (active != nullptr) {
    active->handleEvent(event);
  }
}