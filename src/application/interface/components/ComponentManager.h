#ifndef _COMPONENT_MANAGER_H_
#define _COMPONENT_MANAGER_H_

// forward declaration to avoid circular references
class Application;
class Component;

#include "lvgl.h"

#include "application/interface/components/types/Component.h"
#include "application/workflow/Workflow.h"
#include "events/EventHandler.h"
#include "events/types/InputEvent.h"

class ComponentManager : public EventHandler<InputEvent> {
private:
  Application *app;
  Component *active = nullptr;
  lv_obj_t *screen = nullptr;

public:
  ComponentManager(Application *app) : app(app) {};
  ~ComponentManager() { deleteComponent(); }

  // component lifecycle
  void createComponent(State state);
  void deleteComponent();

  // event handling
  void handleEvent(InputEvent &event);
};

#endif // _COMPONENT_MANAGER_H_
