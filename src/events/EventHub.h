#pragma once

#include "src/events/EventQueue.h"
#include "src/events/types/TouchEvent.h"
#include "src/events/types/WorkflowEvent.h"

class EventHub : public EventHandler<TouchEvent>,
                 public EventHandler<WorkflowEvent> {
private:
  EventQueue<TouchEvent> _touchEventQueue;
  EventQueue<WorkflowEvent> _workflowEventQueue;

public:
  void handleEvent(TouchEvent &event) { _touchEventQueue.post(event); }
  void handleEvent(WorkflowEvent &event) { _workflowEventQueue.post(event); }

  EventQueue<TouchEvent> &touchEvents() { return _touchEventQueue; }
  EventQueue<WorkflowEvent> &workflowEvents() { return _workflowEventQueue; }
};