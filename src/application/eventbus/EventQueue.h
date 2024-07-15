#ifndef _EVENT_QUEUE_H_
#define _EVENT_QUEUE_H_

#include <vector>

#include "src/application/eventbus/EventHandler.h"

// basic generic message queue
template <typename T> class EventQueue {
private:
  std::vector<T> queue;
  std::vector<EventHandler<T> *> eventHandlerQueue;

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
};

#endif // _EVENT_QUEUE_H_