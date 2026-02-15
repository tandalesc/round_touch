#ifndef _STATE_CHANGE_RULE_H_
#define _STATE_CHANGE_RULE_H_

#include "events/types/TouchEvent.h"

struct StateChangeRule {
  TouchType type = TouchType::UnknownType;
  SwipeDirection direction = SwipeDirection::UnknownDirection;
  TouchRegion region = {};
  State newState;
};

static StateChangeRule onSwipe(SwipeDirection direction, State state) {
  return {
    .type = TouchType::SwipeType,
    .direction = direction,
    .newState = state
  };
}
static StateChangeRule onSwipeUp(State state) {
  return onSwipe(SwipeDirection::SwipeUp, state);
}
static StateChangeRule onSwipeDown(State state) {
  return onSwipe(SwipeDirection::SwipeDown, state);
}
static StateChangeRule onSwipeLeft(State state) {
  return onSwipe(SwipeDirection::SwipeLeft, state);
}
static StateChangeRule onSwipeRight(State state) {
  return onSwipe(SwipeDirection::SwipeRight, state);
}

static StateChangeRule onTap(TouchRegion region, State state) {
  return {
    .type = TouchType::TapType,
    .region = region,
    .newState = state
  };
}
static StateChangeRule onTapAnywhere(State state) { return onTap({}, state); }

#endif // _STATE_CHANGE_RULE_H_