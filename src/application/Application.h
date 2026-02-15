#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "device/Device.h"

#include "application/interface/Interface.h"
#include "application/services/HomeAssistant.h"
#include "application/workflow/Workflow.h"

#include "events/EventHub.h"

class Application {
private:
  Device *_device;
  Workflow _workflow;
  Interface _interface;
  EventHub _eventhub;
  HomeAssistant *_ha = nullptr;

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
  HomeAssistant *ha();
};

#endif // _APPLICATION_H_