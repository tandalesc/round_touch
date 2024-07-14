#include <Arduino.h>

#include "src/application/eventbus/EventBus.h"

void EventBus::notify(EventType type, Event event) {
  CallbackQueue &queue = this->getCallbackQueue(event.type);
  for (auto callback : queue) {
    callback(event);
  }
}

void EventBus::addEvent(EventFormat e) {
  Event event = {.type = e.type, .timestamp = millis()};
  EventQueue &queue = this->getEventQueue(event.type);
  queue.push_back(event);
  this->notify(event.type, event);
}

void EventBus::subscribe(EventType type, EventCallback callback) {
  this->getCallbackQueue(type).push_back(callback);
}

CallbackQueue &EventBus::getCallbackQueue(EventType &type) {
  return this->callbackQueues[int(type)];
}

EventQueue &EventBus::getEventQueue(EventType &type) {
  return this->eventQueues[int(type)];
}