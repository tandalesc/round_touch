#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "src/device/Device.h"

#include "src/application/workflow/Workflow.h"
#include "src/application/interface/Interface.h"

class Application {
private:
  Device *_device;
  Workflow _workflow;
  Interface _interface;

public:
  Application(Device *device) : _device(device) {};
  void init();
  void loop();

  Device *device();
  Workflow &workflow();
  Interface &interface();
};

#endif // _APPLICATION_H_