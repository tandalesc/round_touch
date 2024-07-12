#include "src/application/Application.h"
#include "src/application/workflow/WorkflowStates.h"

Device *Application::device() { return this->_device; }
Workflow &Application::workflow() { return this->_workflow; }

void Application::init() {
  this->workflow().navigate(READY);
  Serial.println("Initialized Application.");
}

void Application::loop() { this->process(); }

void Application::process() {
  Workflow &workflow = this->workflow();
  bool render = false;
  if (workflow.hasChanges()) {
    workflow.applyChanges();
    render = true;
  }
  RenderFunction renderView = processState(this, workflow.getState());
  if (render) {
    Serial.println("Something changed, re-rendering.");
    renderView(this);
  }
}
