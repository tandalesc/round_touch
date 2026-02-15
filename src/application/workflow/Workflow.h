#ifndef _WORKFLOW_H_
#define _WORKFLOW_H_

#include "util/Timer.h"

class Application;

enum State { NOT_STARTED, ERROR, READY, INFO1, INFO2, INFO3, ECOMODE, DETAILS, LIGHTS };

class Workflow {
private:
  Application *app;
  State state = NOT_STARTED;
  State prevState = NOT_STARTED;
  Timer debounceTimer{100};

public:
  Workflow(Application *app) : app(app) {};

  State getState();
  bool canNavigate(State newState);
  void navigate(State newState);
};

#endif // _WORKFLOW_H