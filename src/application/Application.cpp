#include "src/application/Application.h"

Device *Application::device() { return this->_device; }
Workflow &Application::workflow() { return this->_workflow; }
Interface &Application::interface() { return this->_interface; }
EventQueue<TouchEvent> &Application::touchEvents() {
  return this->_touchEventQueue;
}

void Application::init() {
  this->workflow().navigate(READY);
  Serial.println("Initialized Application.");
}

void Application::loop() {
  // refresh interface
  this->interface().process(this);
}