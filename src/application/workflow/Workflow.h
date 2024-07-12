#ifndef _WORKFLOW_H_
#define _WORKFLOW_H_

enum State { NOT_STARTED, ERROR, READY, INFO1, INFO2, INFO3, ECOMODE, DETAILS };

class Workflow {
private:
  State state = NOT_STARTED;
  State prevState = NOT_STARTED;
  bool changed = false;
  // prevent making multiple conflicting navigate calls
  unsigned long pauseTimer = 0;
  void pauseNavigation();

public:
  State getState();
  bool canNavigate(State newState);
  void navigate(State newState);
  void start();

  bool hasChanges();
  void applyChanges();
};

#endif // _WORKFLOW_H