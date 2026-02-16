#ifndef _SCREENS_H_
#define _SCREENS_H_

#include "application/interface/components/types/Component.h"
#include "application/workflow/Workflow.h"

// System screens (always compiled)
RenderableComponent DeviceNotStarted();
RenderableComponent ErrorState();
RenderableComponent SystemShadeState();

// Fallback for user screens when no JSON manifest is loaded
RenderableComponent userScreenFallback(State state);

#endif // _SCREENS_H_
