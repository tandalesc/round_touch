#ifndef _EVENT_QUEUE_H_
#define _EVENT_QUEUE_H_

#include <list>

#include "src/events/EventHandler.h"

// basic generic message queue
template <typename T> class EventQueue : public EventHandler<T> {
private:
  std::list<T> queue;
  std::list<EventHandler<T> *> eventHandlerQueue;

  void notify(T &event) {
    for (EventHandler<T> *eventHandler : eventHandlerQueue) {
      eventHandler->handleEvent(event);
    }
  }

public:
  void post(T event) {
    queue.push_back(event);
    notify(event);
  }

  void subscribe(EventHandler<T> *eventHandler) {
    eventHandlerQueue.push_back(eventHandler);
  }

  void unsubscribe(EventHandler<T> *eventHandler) {
    eventHandlerQueue.remove(eventHandler);
  }

  void handleEvent(T &event) { post(event); }
};

#endif // _EVENT_QUEUE_H_