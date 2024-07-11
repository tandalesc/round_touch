#ifndef _WORKFLOW_H_
#define _WORKFLOW_H_

enum State { NOT_STARTED, READY, ERROR };

class Workflow {
private:
  State state = NOT_STARTED;

public:
  State getState();
  bool canNavigate(State newState);
  void navigate(State newState);
};

#endif // _WORKFLOW_H