#ifndef _SWIPE_INPUT_COMPONENT_H_
#define _SWIPE_INPUT_COMPONENT_H_

#include "src/application/eventbus/types/TouchEvent.h"
#include "src/application/interface/components/types/Component.h"

class SwipeInput : public Component {
public:
  void setupEventListeners(Application *app) override {
    app->touchEvents().subscribe(&printTouchEvent);
  }
};

#endif // _SWIPE_INPUT_COMPONENT_H_