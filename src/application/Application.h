#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "device/Device.h"

#include "application/interface/Interface.h"
#include "application/services/HomeAssistant.h"
#include "application/services/OTAUpdate.h"
#include "application/workflow/Workflow.h"
#include "ui/registry/ComponentRegistry.h"
#include "ui/registry/UserScreenManager.h"

#include "events/EventHub.h"

class Application {
private:
  Device *_device;
  Workflow _workflow;
  Interface _interface;
  EventHub _eventhub;
  HomeAssistant *_ha = nullptr;
  OTAUpdate *_ota = nullptr;
  ComponentRegistry _registry;
  UserScreenManager _userScreenManager;

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
  OTAUpdate *ota();
  ComponentRegistry &registry();
  UserScreenManager &userScreenManager();
};

#endif // _APPLICATION_H_