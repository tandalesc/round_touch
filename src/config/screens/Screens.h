#ifndef _SCREENS_H_
#define _SCREENS_H_

#include "application/interface/components/types/Component.h"
#include "application/workflow/Workflow.h"

RenderableComponent DeviceNotStarted();
RenderableComponent ErrorState();
RenderableComponent ReadyState();
RenderableComponent EcoModeState();
RenderableComponent Info1State();
RenderableComponent Info2State();
RenderableComponent Info3State();
RenderableComponent DetailsState();
RenderableComponent LightsState();

#endif // _SCREENS_H_
