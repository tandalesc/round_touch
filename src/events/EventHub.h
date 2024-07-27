#pragma once

#include "src/events/EventQueue.h"
#include "src/events/types/TouchEvent.h"
#include "src/events/types/WorkflowEvent.h"

class EventHub : public EventHandler<WorkflowEvent> {
private:
  EventQueue<WorkflowEvent> _workflowEventQueue;

public:
  void handleEvent(TouchEvent &event) { _touchEventQueue.post(event); }
  void handleEvent(WorkflowEvent &event) { _workflowEventQueue.post(event); }

  EventQueue<WorkflowEvent> &workflowEvents() { return _workflowEventQueue; }
};