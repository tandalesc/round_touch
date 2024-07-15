#ifndef _COMPONENT_MANAGER_H_
#define _COMPONENT_MANAGER_H_

// forward declaration to avoid circular references
class Application;
class Component;

#include "src/application/interface/components/types/Component.h"
#include "src/application/interface/components/types/Layout.h"
#include "src/application/workflow/Workflow.h"

class ComponentManager {
private:
  Application *app;
  Component *active = nullptr;
  LayoutContext rootLayout = {.position = {.t = 80, .l = 10}};

public:
  ComponentManager(Application *app) : app(app) {};
  ~ComponentManager() { deleteComponent(); }

  void createComponent(State state);
  void renderComponent();
  void deleteComponent();
};

#endif // _COMPONENT_MANAGER_H_