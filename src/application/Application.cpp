#include "src/application/Application.h"
#include "src/application/workflow/WorkflowStates.h"

Application::Application(Device *device) { this->_device = device; }

Device *Application::device() { return this->_device; }
Workflow &Application::workflow() { return this->_workflow; }

void Application::render() {
  renderWorkflowState(this);
  delay(500);
}

void Application::ready() {
  auto gfx = this->device()->display().gfx;
  gfx->fillScreen(BLACK);
  this->device()->showMessage("DETAILS");
  this->workflow().navigate(READY);
}