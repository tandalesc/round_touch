#include <Arduino.h>

#include "application/Application.h"
#include "application/workflow/Workflow.h"
#include "events/types/WorkflowEvent.h"

State Workflow::getState() { return this->state; }

bool Workflow::canNavigate(State newState) {
  if (newState == NOT_STARTED) {
    return false;
  }
  if (debounceTimer.running()) {
    return false;
  }
  return newState != this->state;
}

void Workflow::navigate(State newState) {
  if (!canNavigate(newState)) {
    return;
  }
  debounceTimer.start();
  prevState = state;
  state = newState;
  WorkflowEvent event = {.from = prevState, .to = state, .timestamp = millis()};
  app->eventhub().handleEvent(event);
}