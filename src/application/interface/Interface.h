#ifndef _INTERFACE_H_
#define _INTERFACE_H_

// forward declaration to avoid circular references
class Application;

#include "src/application/interface/components/ComponentManager.h"

class Interface {
private:
  Application *app;
  ComponentManager *manager = new ComponentManager(app);

public:
  Interface(Application *app) : app(app) {};
  ~Interface() { delete manager; }
  void deferredProcess();
  void immediateProcess();
};

#endif // _INTERFACE_H_