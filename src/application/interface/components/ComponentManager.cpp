#include "application/Application.h"

#include "application/interface/components/ComponentManager.h"
#include "application/workflow/ComponentLibrary.h"

void ComponentManager::createComponent(State state) {
  // if we are already assigned to a component, destroy it first
  if (active != nullptr) {
    deleteComponent();
  }
  active = createComponentFromState(state);
  active->attachApplication(app);
}

void ComponentManager::renderComponent() {
  if (active != nullptr) {
    active->calculateSize(rootLayout);
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

void ComponentManager::handleEvent(InputEvent &event) {
  if (active != nullptr) {
    active->handleEvent(event);
  }
}