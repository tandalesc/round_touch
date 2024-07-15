#ifndef _STATE_CHANGE_RULE_H_
#define _STATE_CHANGE_RULE_H_

#include "src/application/eventbus/types/TouchEvent.h"

struct StateChangeRule {
  TouchEvent::Type type = TouchEvent::Unknown;
  TouchEvent::Direction direction = TouchEvent::None;
  TouchRegion region = {};
  State newState;
};

static StateChangeRule onSwipe(TouchEvent::Direction direction, State state) {
  return {.type = TouchEvent::Swipe, .direction = direction, .newState = state};
}
static StateChangeRule onSwipeUp(State state) {
  return onSwipe(TouchEvent::Up, state);
}
static StateChangeRule onSwipeDown(State state) {
  return onSwipe(TouchEvent::Down, state);
}
static StateChangeRule onSwipeLeft(State state) {
  return onSwipe(TouchEvent::Left, state);
}
static StateChangeRule onSwipeRight(State state) {
  return onSwipe(TouchEvent::Right, state);
}

static StateChangeRule onTap(TouchRegion region, State state) {
  return {.type = TouchEvent::Tap, .region = region, .newState = state};
}
static StateChangeRule onTapAnywhere(State state) { return onTap({}, state); }

#endif // _STATE_CHANGE_RULE_H_