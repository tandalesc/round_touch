#pragma once

enum class EventType { UnknownEvent, SyntheticEvent, UserInputEvent };
enum class InputType { UnknownInput, TouchInput, ButtonInput };

struct InputEvent {
  EventType eventType;
  InputType inputType;

  InputEvent(EventType eventType, InputType inputType)
      : eventType(eventType), inputType(inputType) {};

  virtual void debug() {
    Serial.print("InputEvent: Received ");
    if (inputType == InputType::TouchInput) {
      Serial.print("touch input ");
    } else if (inputType == InputType::ButtonInput) {
      Serial.print("button input ");
    } else {
      Serial.println("unknown input ");
    }
    Serial.print("event from ");
    if (eventType == EventType::SyntheticEvent) {
      Serial.println("the system.");
    } else if (eventType == EventType::UserInputEvent) {
      Serial.println("the user.");
    } else {
      Serial.println("an unknown source.");
    }
  }
};