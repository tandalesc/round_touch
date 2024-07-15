#include <Arduino.h>

#include "src/application/Application.h"
#include "src/application/eventbus/types/WorkflowEvent.h"
#include "src/application/workflow/Workflow.h"

State Workflow::getState() { return this->state; }

bool Workflow::canNavigate(State newState) {
  if (newState == NOT_STARTED) {
    return false;
  }
  if (millis() < debounceTimerMs) {
    return false;
  }
  return newState != this->state;
}

void Workflow::navigate(State newState) {
  if (!canNavigate(newState)) {
    return;
  }
  pauseNavigation();
  prevState = state;
  state = newState;
  app->workflowEvents().post(
      {.from = prevState, .to = state, .timestamp = millis()});
}

// prevent navigation from firing repeatedly
void Workflow::pauseNavigation() {
  debounceTimerMs = millis() + debounceTimeoutMs;
}