#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include "src/application/Application.h"

#include "src/application/interface/components/types/Component.h"

RenderableComponent DeviceNotStarted();
RenderableComponent ErrorState();
RenderableComponent ReadyState();
RenderableComponent EcoModeState();
RenderableComponent Info1State();
RenderableComponent Info2State();
RenderableComponent Info3State();
RenderableComponent DetailsState();

#endif // _COMPONENTS_H_