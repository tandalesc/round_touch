#include "src/application/workflow/Workflow.h"

State Workflow::getState() { return state; }

bool Workflow::canNavigate(State newState) {
  State currentState = this->state;
  if (newState == NOT_STARTED) {
    return false;
  }
  return newState != currentState;
}

void Workflow::navigate(State newState) {
  if (this->canNavigate(newState)) {
    this->state = newState;
  }
}