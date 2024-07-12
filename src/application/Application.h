#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "src/device/Device.h"

#include "src/application/workflow/Workflow.h"

class Application {
private:
  Device *_device;
  Workflow _workflow;
  void process();

public:
  Application(Device *device) : _device(device) {};
  void init();
  void loop();

  Device *device();
  Workflow &workflow();
};

typedef void (*RenderFunction)(Application *);

#endif // _APPLICATION_H_