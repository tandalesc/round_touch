#ifndef _INTERFACE_H_
#define _INTERFACE_H_

// forward declaration to avoid circular references
class Application;

#include "src/application/eventbus/EventHandler.h"
#include "src/application/eventbus/types/TouchEvent.h"
#include "src/application/interface/components/ComponentManager.h"

class Interface : public EventHandler<TouchEvent> {
private:
  Application *app;
  ComponentManager *manager;

public:
  Interface(Application *app);
  ~Interface();

  void deferredProcess();
  void immediateProcess();
  void handleEvent(TouchEvent &event);
};

#endif // _INTERFACE_H_