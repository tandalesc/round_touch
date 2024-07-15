#include "src/application/Application.h"

#include "src/application/interface/components/ComponentManager.h"
#include "src/application/interface/components/Components.h"

void ComponentManager::createComponent(State state) {
  // if we are already assigned to a component, destroy it first
  if (active != nullptr) {
    deleteComponent();
  }
  // pick the right component based on state
  if (state == NOT_STARTED) {
    active = DeviceNotStarted();
  } else if (state == ERROR) {
    active = ErrorState();
  } else if (state == READY) {
    active = ReadyState();
  } else if (state == ECOMODE) {
    active = EcoModeState();
  } else if (state == INFO1) {
    active = Info1State();
  } else if (state == INFO2) {
    active = Info2State();
  } else if (state == INFO3) {
    active = Info3State();
  } else if (state == DETAILS) {
    active = DetailsState();
  }
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