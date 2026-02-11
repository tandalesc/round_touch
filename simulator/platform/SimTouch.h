#ifndef _SIM_TOUCH_H_
#define _SIM_TOUCH_H_

#include <SDL2/SDL.h>
#include <string>
#include <utility>

class SimTouch {
public:
  static void processEvent(const SDL_Event &event);
  static bool hasPendingGesture();
  static std::string consumeGesture();
  static std::pair<int, int> getGesturePos();
};

#endif // _SIM_TOUCH_H_
