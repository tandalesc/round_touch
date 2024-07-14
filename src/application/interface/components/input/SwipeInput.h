#ifndef _SWIPE_INPUT_COMPONENT_H_
#define _SWIPE_INPUT_COMPONENT_H_

#include "src/application/eventbus/types/TouchEvent.h"
#include "src/application/interface/components/types/Component.h"

static void handleEvent(TouchEvent &event) {
  if (event.type == TouchEvent::Tap) {
    Serial.print("Received Tap at ");
    Serial.print(event.location.x);
    Serial.print(", ");
    Serial.print(event.location.y);
    Serial.println();
  }
}

class SwipeInput : public Component {
public:
  void setupEventListeners(Application *app) override {
    app->touchEvents().subscribe(&handleEvent);
  }
};

#endif // _SWIPE_INPUT_COMPONENT_H_