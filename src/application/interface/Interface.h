#ifndef _INTERFACE_H_
#define _INTERFACE_H_

// forward declaration to avoid circular references
class Application;

#include "application/interface/components/ComponentManager.h"
#include "events/EventHandler.h"
#include "events/types/InputEvent.h"
#include "events/types/WorkflowEvent.h"

class Interface : public EventHandler<InputEvent>,
                  public EventHandler<WorkflowEvent> {
private:
  Application *app;
  ComponentManager *manager;
  bool refresh = false;

public:
  Interface(Application *app);
  ~Interface();

  void loop();
  void handleEvent(InputEvent &event);
  void handleEvent(WorkflowEvent &event);
};

#endif // _INTERFACE_H_