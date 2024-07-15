#include <Arduino.h>

#include "src/application/Application.h"

Device *Application::device() { return this->_device; }
Workflow &Application::workflow() { return this->_workflow; }
Interface &Application::interface() { return this->_interface; }
EventQueue<TouchEvent> &Application::touchEvents() {
  return this->_touchEventQueue;
}

void Application::init() {
  touchEvents().subscribe(&interface());
  workflow().navigate(READY);
  Serial.println("Initialized Application.");
}

void Application::loop() {  
  processTouchEvents();
  interface().deferredProcess();
}

// reads all touch events and adds them to the event queue.
// not compatible with immediate processing, as this method can
// compete with component code for input events. this should
// instead only be used with deferred processing and input listeners.
void Application::processTouchEvents() {
  TouchScreen &touch = device()->touchscreen();
  if (touch.available()) {
    TouchEvent event = {.type = TouchEvent::Unknown, .timestamp = millis()};
    String gesture = touch.gesture();
    if (gesture == "SINGLE CLICK") {
      event.type = TouchEvent::Tap;
      event.location = touch.location();
    } else if (gesture == "SWIPE UP" || gesture == "SWIPE DOWN" ||
               gesture == "SWIPE LEFT" || gesture == "SWIPE RIGHT") {
      event.type = TouchEvent::Swipe;
      if (gesture == "SWIPE UP") {
        // swap down and up to match hardware
        event.direction = TouchEvent::Down;
      } else if (gesture == "SWIPE DOWN") {
        event.direction = TouchEvent::Up;
      } else if (gesture == "SWIPE LEFT") {
        event.direction = TouchEvent::Left;
      } else if (gesture == "SWIPE RIGHT") {
        event.direction = TouchEvent::Right;
      }
    }
    touchEvents().post(event);
  }
}