#ifndef _WORKFLOW_H_
#define _WORKFLOW_H_

#include "util/Timer.h"

class Application;

// State is an int — system states are named constants 0-31,
// user states are 32+ assigned by JSON manifest at runtime.
using State = int;

// System states (compiled, immutable)
constexpr State NOT_STARTED   = 0;
constexpr State ERROR         = 1;
constexpr State SYSTEM_SHADE  = 2;
constexpr State GO_BACK       = 3; // sentinel: navigate() calls goBack()

// User states start at 32 — IDs come from the server manifest
constexpr State USER_STATE_BASE = 32;

inline bool isSystemState(State s) { return s >= 0 && s < USER_STATE_BASE; }

class Workflow {
private:
  Application *app;
  State state = NOT_STARTED;
  State prevState = NOT_STARTED;
  State prevUserState = USER_STATE_BASE; // last user screen (for shade return)
  Timer debounceTimer{100};

public:
  Workflow(Application *app) : app(app) {};

  State getState();
  bool canNavigate(State newState);
  void navigate(State newState);
  void goBack();
};

#endif // _WORKFLOW_H_
