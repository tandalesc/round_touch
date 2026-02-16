#include <Arduino.h>

#include "application/Application.h"
#include "application/interface/Toast.h"
#include "config/NetworkConfig.h"
#include "config/Version.h"

Device *Application::device() { return _device; }
Workflow &Application::workflow() { return _workflow; }
Interface &Application::interface() { return _interface; }
EventHub &Application::eventhub() { return _eventhub; }
HomeAssistant *Application::ha() { return _ha; }
OTAUpdate *Application::ota() { return _ota; }

void Application::init() {
  // subscribe interface to workflow events
  // the interface will directly receive event information
  // as soon as it happens without needing to periodically
  // refresh.
  eventhub().workflowEvents().subscribe(&interface());
  // initialize Home Assistant service if network is available
  if (device()->network().isConnected()) {
    _ha = new HomeAssistant(&device()->network(), HA_BASE_URL, HA_ACCESS_TOKEN);
    Serial.println("Home Assistant service initialized.");
  } else {
    Serial.println("Network not connected — HA service unavailable.");
  }
  // initialize OTA update service if network is available
  if (device()->network().isConnected()) {
    _ota = new OTAUpdate(&device()->network(), OTA_UPDATE_URL, OTA_SECRET_KEY);
    Serial.println("OTA update service initialized.");
  }
  // kick start application by navigating to first user screen
  // (will be replaced with manifest default_screen once JSON pipeline is wired)
  workflow().navigate(USER_STATE_BASE);
  // check for firmware updates on boot and show toast if available
  if (_ota != nullptr && _ota->checkForUpdate()) {
    char msg[64];
    snprintf(msg, sizeof(msg), "Firmware v%s available",
             _ota->availableVersion().c_str());
    Toast::show(msg, {
      .label = "Update",
      .callback = [](void *ctx) {
        auto *self = static_cast<Application *>(ctx);
        self->workflow().navigate(SYSTEM_SHADE);
      },
      .userData = this,
    });
  }
  Serial.println("Initialized Application.");
}

Application::~Application() {
  // unsubscribe interface from events at the end of lifecycle
  eventhub().workflowEvents().unsubscribe(&interface());
  delete _ota;
  delete _ha;
}

void Application::loop() {
  // our device has a touch screen which produces
  // touch events. since this is a very high volume event stream
  // poll for new events and pass them to the interface
  // when we have processing time, instead of subscribing
  // directly to the event stream.
  device()->touchscreen().pollEvent(&interface());
  // if there is anything new to show, refresh the interface
  interface().loop();
  // sleep for a bit, we don't need immediate updates
  delay(20);
}