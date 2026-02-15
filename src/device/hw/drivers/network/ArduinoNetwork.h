#ifndef _ARDUINO_NETWORK_H_
#define _ARDUINO_NETWORK_H_

#include "device/INetwork.h"

class ArduinoNetwork : public INetwork {
public:
  void init() override;
  bool isConnected() override;
  HttpResponse get(const char *url,
                   const char *authHeader = nullptr) override;
  HttpResponse post(const char *url, const char *body,
                    const char *contentType = "application/json",
                    const char *authHeader = nullptr) override;
};

#endif // _ARDUINO_NETWORK_H_
