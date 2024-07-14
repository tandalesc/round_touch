#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "src/device/Device.h"

#include "src/application/workflow/Workflow.h"
#include "src/application/interface/Interface.h"

#include "src/application/eventbus/EventQueue.h"
#include "src/application/eventbus/types/TouchEvent.h"

class Application {
private:
  Device *_device;
  Workflow _workflow;
  Interface _interface;
  EventQueue<TouchEvent> _touchEventQueue;

public:
  Application(Device *device) : _device(device) {};
  void init();
  void loop();

  Device *device();
  Workflow &workflow();
  Interface &interface();
  EventQueue<TouchEvent> &touchEvents();
};

#endif // _APPLICATION_H_