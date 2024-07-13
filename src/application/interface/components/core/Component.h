#ifndef _CORE_COMPONENT_H_
#define _CORE_COMPONENT_H_

#include "src/application/Application.h"

#define E(component, args...) new component(args)

struct Component {
  virtual void render(Application *app) = 0;
  virtual ~Component() {};
};

typedef Component *RenderableComponent;

struct EmptyComponent : public Component {
  void render(Application *app) override {};
};

#endif // _COMPONENT_H_