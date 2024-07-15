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
    active = ErrorState(app);
  } else if (state == READY) {
    active = ReadyState(app);
  } else if (state == ECOMODE) {
    active = EcoModeState(app);
  } else if (state == INFO1) {
    active = Info1State(app);
  } else if (state == INFO2) {
    active = Info2State(app);
  } else if (state == INFO3) {
    active = Info3State(app);
  } else if (state == DETAILS) {
    active = DetailsState(app);
  }
}

void ComponentManager::renderComponent() {
  active->calculateSize();
  active->updateLayout(rootLayout);
  active->setupEventListeners(app);
  active->render(app);
}

void ComponentManager::deleteComponent() {
  if (active != nullptr) {
    delete active;
    active = nullptr;
  }
}