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
        if (sevent.direction != rule.direction) continue;
        // Vertical swipes only navigate when started from edge zones
        // (top/bottom 15% of screen), like iOS/Android system gestures.
        // This lets interior swipes scroll content instead.
        bool isVertical = sevent.direction == SwipeDirection::SwipeUp ||
                          sevent.direction == SwipeDirection::SwipeDown;
        if (isVertical && app != nullptr) {
          int h = app->device()->display().height();
          int edgeZone = h * 15 / 100;
          int y = sevent.startLocation.y;
          bool fromEdge = (y < edgeZone) || (y > h - edgeZone);
          navigate = fromEdge;
        } else {
          navigate = true;
        }
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