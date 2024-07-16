#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

template <typename T> class EventHandler {
public:
  virtual void handleEvent(T &event) = 0;
};

#endif // _EVENT_HANDLER_H_