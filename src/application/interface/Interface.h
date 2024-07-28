#ifndef _INTERFACE_H_
#define _INTERFACE_H_

// forward declaration to avoid circular references
class Application;

#include "src/application/interface/components/ComponentManager.h"
#include "src/events/EventHandler.h"
#include "src/events/types/InputEvent.h"
#include "src/events/types/WorkflowEvent.h"

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