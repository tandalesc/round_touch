#ifndef _EVENTBUS_H_
#define _EVENTBUS_H_

#include <vector>

#include "src/application/eventbus/types/Event.h"

typedef void (*EventCallback)(Event &);
typedef std::vector<Event> EventQueue;
typedef std::vector<EventCallback> CallbackQueue;

class EventBus {
  EventQueue eventQueues[16];
  CallbackQueue callbackQueues[16];
  void notify(EventType type, Event event);

public:
  void addEvent(EventFormat e);
  void subscribe(EventType type, EventCallback callback);
  EventQueue &getQueue(EventType type);
};

#endif // _EVENTBUS_H_