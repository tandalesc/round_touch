#ifndef _TOUCH_INPUT_COMPONENT_H_
#define _TOUCH_INPUT_COMPONENT_H_

#include <vector>

#include "src/application/eventbus/types/TouchEvent.h"
#include "src/application/interface/components/types/Component.h"

struct StateChangeRule {
  TouchEvent::Type type = TouchEvent::Unknown;
  TouchEvent::Direction direction = TouchEvent::None;
  TouchRegion region = {};
  State newState;
};

class TouchInput : public Component {
  std::vector<StateChangeRule> rules;

public:
  template <typename... T> TouchInput(T... rules) : rules{rules...} {};

  void handleEvent(TouchEvent &event) {
    for (StateChangeRule &rule : rules) {
      if (event.type != rule.type) {
        continue;
      }
      bool navigate = false;
      if (event.type == TouchEvent::Swipe) {
        navigate = event.direction == rule.direction;
      } else if (event.type == TouchEvent::Tap) {
        navigate = rule.region.contains(event.location);
      }
      if (navigate) {
        app->workflow().navigate(rule.newState);
      }
    }
  }
};

#endif // _TOUCH_INPUT_COMPONENT_H_