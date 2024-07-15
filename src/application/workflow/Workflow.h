#ifndef _WORKFLOW_H_
#define _WORKFLOW_H_

class Application;

enum State { NOT_STARTED, ERROR, READY, INFO1, INFO2, INFO3, ECOMODE, DETAILS };

class Workflow {
private:
  Application *app;
  State state = NOT_STARTED;
  State prevState = NOT_STARTED;
  // debounce navigate calls
  const unsigned long debounceTimeoutMs = 100;
  unsigned long debounceTimerMs = 0;
  void pauseNavigation();

public:
  Workflow(Application *app): app(app) {};
  
  State getState();
  bool canNavigate(State newState);
  void navigate(State newState);
};

#endif // _WORKFLOW_H