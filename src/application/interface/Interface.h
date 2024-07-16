#ifndef _INTERFACE_H_
#define _INTERFACE_H_

// forward declaration to avoid circular references
class Application;

#include "src/events/EventHandler.h"
#include "src/events/types/TouchEvent.h"
#include "src/events/types/WorkflowEvent.h"
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

  void loop();
  void handleEvent(TouchEvent &event);
  void handleEvent(WorkflowEvent &event);
};

#endif // _INTERFACE_H_