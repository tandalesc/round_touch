#ifndef _COMPONENT_MANAGER_H_
#define _COMPONENT_MANAGER_H_

// forward declaration to avoid circular references
class Application;
class Component;

#include "src/config/Constants.h"

#include "src/application/interface/components/types/Component.h"
#include "src/application/interface/components/types/Layout.h"
#include "src/application/workflow/Workflow.h"
#include "src/events/EventHandler.h"
#include "src/events/types/InputEvent.h"

class ComponentManager : public EventHandler<InputEvent> {
private:
  Application *app;
  Component *active = nullptr;
  LayoutContext rootLayout = {
      .position = {.t = 40, .b = 40, .l = 12, .r = 12},
      .preferredSize = {.width = SCREEN_MAX_WIDTH - (12 * 2),
                        .height = SCREEN_MAX_HEIGHT - (40 * 2)}};

public:
  ComponentManager(Application *app) : app(app) {};
  ~ComponentManager() { deleteComponent(); }

  // component lifecycle
  void createComponent(State state);
  void renderComponent();
  void deleteComponent();

  // event handling
  void handleEvent(InputEvent &event);
};

#endif // _COMPONENT_MANAGER_H_