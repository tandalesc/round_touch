#ifndef _INTERFACE_H_
#define _INTERFACE_H_

// forward declaration to avoid circular references
class Application;

#include "src/application/eventbus/EventHandler.h"
#include "src/application/eventbus/types/TouchEvent.h"
#include "src/application/eventbus/types/WorkflowEvent.h"
#include "src/application/interface/components/ComponentManager.h"

class Interface : public EventHandler<TouchEvent>,
                  public EventHandler<WorkflowEvent> {
private:
  Application *app;
  ComponentManager *manager;
  bool refresh = false;
  
public:
  Interface(Application *app);
  ~Interface();

  void refreshInterface();
  void handleEvent(TouchEvent &event);
  void handleEvent(WorkflowEvent &event);
};

#endif // _INTERFACE_H_