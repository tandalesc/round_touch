#ifndef _TOUCH_INPUT_COMPONENT_H_
#define _TOUCH_INPUT_COMPONENT_H_

#include <initializer_list>
#include <vector>

#include "application/Application.h"
#include "application/interface/components/input/StateChangeRule.h"
#include "application/interface/components/types/Component.h"
#include "events/types/TouchEvent.h"

class TouchNavigation : public Component {
  std::vector<StateChangeRule> rules;

public:
  TouchNavigation(StateChangeRule rule) : rules{rule} {};
  TouchNavigation(std::initializer_list<StateChangeRule> rules)
      : rules(rules) {};

  void handleEvent(InputEvent &event) {
    if (event.inputType != InputType::TouchInput) {
      return;
    }
    TouchEvent &tevent = static_cast<TouchEvent &>(event);
    for (StateChangeRule &rule : rules) {
      if (tevent.type != rule.type) {
        continue;
      }
      bool navigate = false;
      if (tevent.type == TouchType::SwipeType) {
        SwipeTouchEvent &sevent = static_cast<SwipeTouchEvent &>(event);
        navigate = sevent.direction == rule.direction;
      } else if (tevent.type == TouchType::TapType) {
        TapTouchEvent &ttevent = static_cast<TapTouchEvent &>(event);
        navigate = rule.region.contains(ttevent.location);
      }
      if (navigate) {
        app->workflow().navigate(rule.newState);
        return;
      }
    }
  }
};

#endif // _TOUCH_INPUT_COMPONENT_H_