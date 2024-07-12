#ifndef _WORKFLOW_H_
#define _WORKFLOW_H_

enum State { NOT_STARTED, ERROR, READY, INFO1, INFO2, INFO3, ECOMODE, DETAILS };

class Workflow {
private:
  State state = NOT_STARTED;

public:
  State getState();
  bool canNavigate(State newState);
  void navigate(State newState);
};

#endif // _WORKFLOW_H