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
  // GO_BACK sentinel — redirect to the last user screen
  if (newState == GO_BACK) {
    navigate(prevUserState);
    return;
  }
  if (!canNavigate(newState)) {
    return;
  }
  debounceTimer.start();

  // Remember current user screen before entering system shade
  if (!isSystemState(state)) {
    prevUserState = state;
  }

  prevState = state;
  state = newState;
  WorkflowEvent event = {.from = prevState, .to = state, .timestamp = millis()};
  app->eventhub().handleEvent(event);
}

void Workflow::goBack() { navigate(prevUserState); }
