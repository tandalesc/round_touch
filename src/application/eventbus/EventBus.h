#ifndef _EVENTBUS_H_
#define _EVENTBUS_H_

#include <vector>

#include "src/application/eventbus/types/Event.h"

typedef void (*EventCallback)(Event &);
typedef std::vector<Event> EventQueue;
typedef std::vector<EventCallback> CallbackQueue;

class EventBus {
private:
  EventQueue eventQueues[16];
  CallbackQueue callbackQueues[16];

  void notify(EventType type, Event event);
  CallbackQueue &getCallbackQueue(EventType &type);

public:
  void addEvent(EventFormat e);
  void subscribe(EventType type, EventCallback callback);
  EventQueue &getEventQueue(EventType &type);
};

#endif // _EVENTBUS_H_