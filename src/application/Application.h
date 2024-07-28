#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "src/device/Device.h"

#include "src/application/interface/Interface.h"
#include "src/application/workflow/Workflow.h"

#include "src/events/EventHub.h"

class Application {
private:
  Device *_device;
  Workflow _workflow;
  Interface _interface;
  EventHub _eventhub;

public:
  Application(Device *device)
      : _device(device), _workflow(this), _interface(this) {};
  ~Application();
  void init();
  void loop();

  Device *device();
  Workflow &workflow();
  Interface &interface();
  EventHub &eventhub();
};

#endif // _APPLICATION_H_