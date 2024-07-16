#ifndef _COMPONENT_LIBRARY_H_
#define _COMPONENT_LIBRARY_H_

#include "src/application/interface/components/types/Component.h"
#include "src/application/workflow/Workflow.h"

RenderableComponent createComponentFromState(State state);

RenderableComponent DeviceNotStarted();
RenderableComponent ErrorState();
RenderableComponent ReadyState();
RenderableComponent EcoModeState();
RenderableComponent Info1State();
RenderableComponent Info2State();
RenderableComponent Info3State();
RenderableComponent DetailsState();

#endif // _COMPONENT_LIBRARY_H_