#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "src/device/Device.h"

#include "src/application/workflow/Workflow.h"

class Application {
private:
  Device *_device;
  Workflow _workflow;

public:
  Application(Device *device);
  void render();
  void ready();

  Device *device();
  Workflow &workflow();
};

#endif // _APPLICATION_H_