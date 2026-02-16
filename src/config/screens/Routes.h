#ifndef _ROUTES_H_
#define _ROUTES_H_

#include "application/interface/components/types/Component.h"
#include "application/workflow/Workflow.h"

class Application;

RenderableComponent createComponentFromState(State state, Application *app);

#endif // _ROUTES_H_
