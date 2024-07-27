#include <Arduino.h>

#include "src/application/Application.h"

Device *Application::device() { return _device; }
Workflow &Application::workflow() { return _workflow; }
Interface &Application::interface() { return _interface; }
EventHub &Application::eventhub() { return _eventhub; }

void Application::init() {
  // subscribe interface to touch and workflow events
  // this way the interface will directly receive events
  // and can handle them directly
  eventhub().touchEvents().subscribe(&interface());
  eventhub().workflowEvents().subscribe(&interface());
  // kick start application by navigating to first state
  workflow().navigate(READY);
  Serial.println("Initialized Application.");
}

Application::~Application() {
  // unsubscribe interface from events at the end of lifecycle
  eventhub().touchEvents().unsubscribe(&interface());
  eventhub().workflowEvents().unsubscribe(&interface());
}

void Application::loop() {
  // poll for new touch events
  device()->pollEvent(eventhub().touchEvents());
  interface().loop();
  // sleep for a bit, we don't need
  // immediate updates
  delay(20);
}