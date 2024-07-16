#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "src/device/Device.h"

#include "src/application/interface/Interface.h"
#include "src/application/workflow/Workflow.h"

#include "src/events/EventQueue.h"
#include "src/events/types/TouchEvent.h"
#include "src/events/types/WorkflowEvent.h"

class Application {
private:
  Device *_device;
  Workflow _workflow;
  Interface _interface;
  EventQueue<TouchEvent> _touchEventQueue;
  EventQueue<WorkflowEvent> _workflowEventQueue;

  void processTouchEvents();

public:
  Application(Device *device)
      : _device(device), _workflow(this), _interface(this) {};
  ~Application();
  void init();
  void loop();

  Device *device();
  Workflow &workflow();
  Interface &interface();
  EventQueue<TouchEvent> &touchEvents();
  EventQueue<WorkflowEvent> &workflowEvents();
};

#endif // _APPLICATION_H_