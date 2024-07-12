#include "src/application/Application.h"
#include "src/application/workflow/WorkflowStates.h"

Device *Application::device() { return this->_device; }
Workflow &Application::workflow() { return this->_workflow; }

void Application::init() {
  this->workflow().start();
  Serial.println("Initialized Application.");
}

void Application::loop() {
  this->process();
}

void Application::process() {
  Workflow &workflow = this->workflow();
  // run state logic without rendering
  processState(this, workflow.getState());
  if (workflow.hasChanges()) {
    Serial.println("Something changed, re-rendering.");
    workflow.applyChanges();
    // render the output this time
    // TODO - only run processState once per loop
    RenderFunction renderView = processState(this, workflow.getState());
    renderView(this);
    // wait
    delay(500);
  }
}
