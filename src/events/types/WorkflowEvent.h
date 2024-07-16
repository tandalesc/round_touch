#ifndef _WORKFLOW_EVENT_T_
#define _WORKFLOW_EVENT_T_

#include "src/application/workflow/Workflow.h"

struct WorkflowEvent {
  enum Type { Update } type = Type::Update;
  State from, to;
  unsigned long timestamp = 0;
};

#endif // _WORKFLOW_EVENT_T_