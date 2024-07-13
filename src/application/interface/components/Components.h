#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include "src/application/Application.h"
#include "src/application/interface/components/core/Component.h"
#include "src/application/interface/components/core/FillScreen.h"
#include "src/application/interface/components/core/Text.h"

RenderableComponent DeviceNotStarted();
RenderableComponent ErrorState(Application *app);
RenderableComponent ReadyState(Application *app);
RenderableComponent EcoModeState(Application *app);
RenderableComponent Info1State(Application *app);
RenderableComponent Info2State(Application *app);
RenderableComponent Info3State(Application *app);
RenderableComponent DetailsState(Application *app);

#endif // _COMPONENTS_H_