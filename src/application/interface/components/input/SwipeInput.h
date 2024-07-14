#ifndef _FILL_SCREEN_COMPONENT_H_
#define _FILL_SCREEN_COMPONENT_H_

#include "src/application/interface/components/types/Component.h"

struct SwipeInputProps {};

class SwipeInput : public Component {
private:
  SwipeInputProps props;

public:
  SwipeInput(SwipeInputProps props) : props(props) {};

  void setupEventListeners(Application *app) override {

  }
};

#endif // _FILL_SCREEN_COMPONENT_H_