#ifndef _INETWORK_H_
#define _INETWORK_H_

#include <Arduino.h>

struct HttpResponse {
  int statusCode = -1;
  String body;
};

class INetwork {
public:
  virtual ~INetwork() = default;
  virtual void init() = 0;
  virtual bool isConnected() = 0;
  virtual HttpResponse get(const char *url,
                           const char *authHeader = nullptr) = 0;
  virtual HttpResponse post(const char *url, const char *body,
                            const char *contentType = "application/json",
                            const char *authHeader = nullptr) = 0;
};

#endif // _INETWORK_H_
