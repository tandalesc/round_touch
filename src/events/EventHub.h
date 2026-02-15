#pragma once

#include "events/EventQueue.h"
#include "events/types/TouchEvent.h"
#include "events/types/WorkflowEvent.h"

class EventHub : public EventHandler<WorkflowEvent> {
private:
  EventQueue<WorkflowEvent> _workflowEventQueue;

public:
  void handleEvent(WorkflowEvent &event) { _workflowEventQueue.post(event); }

  EventQueue<WorkflowEvent> &workflowEvents() { return _workflowEventQueue; }
};