#ifndef _INTERFACE_H_
#define _INTERFACE_H_

// forward declaration to avoid circular references
class Application;

class Interface {
public:
  void process(Application *app);
};

#endif // _INTERFACE_H_