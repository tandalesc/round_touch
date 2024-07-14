#ifndef _EVENTBUS_EVENT_T_
#define _EVENTBUS_EVENT_T_

enum EventType {
  NoEvent,
  TouchEvent,
  WorkflowEvent,
  NetworkEvent,
  BatteryEvent
};

struct EventFormat {
  EventType type;
};

struct Event {
  EventType type;
  unsigned long timestamp;
  int priority = 0;
};

#endif // _EVENTBUS_EVENT_T