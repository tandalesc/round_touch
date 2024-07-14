#ifndef _SWIPE_INPUT_COMPONENT_H_
#define _SWIPE_INPUT_COMPONENT_H_

#include "src/application/interface/components/types/Component.h"

static void handleEvent(Event &event) {
  // todo, other stuff
  Serial.println("That worked somehow");
}

class SwipeInput : public Component {
public:
  void setupEventListeners(Application *app) override {
    app->events().subscribe(EventType::TouchEvent, *handleEvent);
  }
};

#endif // _SWIPE_INPUT_COMPONENT_H_