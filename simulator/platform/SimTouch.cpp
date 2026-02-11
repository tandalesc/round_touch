#include "SimTouch.h"
#include <cmath>
#include <cstdio>

static const int SWIPE_THRESHOLD = 30;

static bool s_mouseDown = false;
static int s_downX = 0, s_downY = 0;
static bool s_pendingGesture = false;
static std::string s_gesture;
static int s_gestureX = 0, s_gestureY = 0;

void SimTouch::processEvent(const SDL_Event &event) {
  if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
    s_mouseDown = true;
    s_downX = event.button.x;
    s_downY = event.button.y;
  } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
    if (!s_mouseDown) return;
    s_mouseDown = false;

    int upX = event.button.x;
    int upY = event.button.y;
    int dx = upX - s_downX;
    int dy = upY - s_downY;
    int dist = (int)std::sqrt(dx * dx + dy * dy);

    if (dist < SWIPE_THRESHOLD) {
      // Tap
      s_gesture = "SINGLE CLICK";
      s_gestureX = s_downX;
      s_gestureY = s_downY;
    } else {
      // Swipe - determine dominant direction
      if (std::abs(dx) > std::abs(dy)) {
        s_gesture = dx > 0 ? "SWIPE RIGHT" : "SWIPE LEFT";
      } else {
        s_gesture = dy > 0 ? "SWIPE DOWN" : "SWIPE UP";
      }
      s_gestureX = s_downX;
      s_gestureY = s_downY;
    }

    s_pendingGesture = true;
    printf("[SimTouch] %s at (%d, %d)\n", s_gesture.c_str(), s_gestureX,
           s_gestureY);
  }
}

bool SimTouch::hasPendingGesture() { return s_pendingGesture; }

std::string SimTouch::consumeGesture() {
  s_pendingGesture = false;
  return s_gesture;
}

std::pair<int, int> SimTouch::getGesturePos() {
  return {s_gestureX, s_gestureY};
}
