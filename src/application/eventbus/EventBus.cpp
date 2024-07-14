#include <Arduino.h>

#include "src/application/eventbus/EventBus.h"

void EventBus::notify(EventType type, Event event) {
  for (auto callback : this->callbackQueues[event.type]) {
    callback(event);
  }
}

void EventBus::addEvent(EventFormat e) {
  Event event = {.type = e.type, .timestamp = millis()};
  EventQueue &queue = this->getQueue(event.type);
  queue.push_back(event);
  this->notify(event.type, event);
}

void EventBus::subscribe(EventType type, EventCallback callback) {
  this->callbackQueues[type].push_back(callback);
}

EventQueue &EventBus::getQueue(EventType type) {
  return this->eventQueues[type];
}