#include <Arduino.h>

#include "src/application/workflow/Workflow.h"

State Workflow::getState() {
  return this->state;
}

bool Workflow::canNavigate(State newState) {
  if (newState == NOT_STARTED) {
    return false;
  }
  return newState != this->state;
}

void Workflow::navigate(State newState) {
  if (this->canNavigate(newState)) {
    this->prevState = this->state;
    this->state = newState;
    this->changed = true;
  }
}

void Workflow::start() {

  this->navigate(READY);
}

bool Workflow::hasChanges() {
  return this->changed;
}

void Workflow::applyChanges() {
  this->changed = false;
  Serial.print(this->prevState);
  Serial.print(" -> ");
  Serial.print(this->state);
  Serial.println();
}