#ifndef _TOUCH_INPUT_COMPONENT_H_
#define _TOUCH_INPUT_COMPONENT_H_

#include <initializer_list>
#include <vector>

#include "src/application/interface/components/types/Component.h"
#include "src/application/interface/components/input/StateChangeRule.h"
#include "src/events/types/TouchEvent.h"

class TouchNavigation : public Component {
  std::vector<StateChangeRule> rules;

public:
  TouchNavigation(StateChangeRule rule) : rules{rule} {};
  TouchNavigation(std::initializer_list<StateChangeRule> rules)
      : rules(rules) {};

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
        return;
      }
    }
  }
};

#endif // _TOUCH_INPUT_COMPONENT_H_