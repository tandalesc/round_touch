#include <Arduino.h>

#include "src/application/Application.h"

Device *Application::device() { return _device; }
Workflow &Application::workflow() { return _workflow; }
Interface &Application::interface() { return _interface; }
EventHub &Application::eventhub() { return _eventhub; }

void Application::init() {
  // subscribe interface to workflow events
  // the interface will directly receive event information
  // as soon as it happens without needing to periodically
  // refresh.
  eventhub().workflowEvents().subscribe(&interface());
  // kick start application by navigating to first state
  workflow().navigate(READY);
  Serial.println("Initialized Application.");
}

Application::~Application() {
  // unsubscribe interface from events at the end of lifecycle
  eventhub().workflowEvents().unsubscribe(&interface());
}

void Application::loop() {
  // our device has a touch screen which produces
  // touch events. since this is a very high volume event stream
  // poll for new events and pass them to the interface
  // when we have processing time, instead of subscribing
  // directly to the event stream.
  device()->touchscreen().pollEvent(&interface());
  // if there is anything new to show, refresh the interface
  interface().loop();
  // sleep for a bit, we don't need immediate updates
  delay(20);
}