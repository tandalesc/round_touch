#ifndef _COMPONENT_TYPES_H_
#define _COMPONENT_TYPES_H_

#include "src/application/Application.h"
#include "src/application/interface/components/types/Layout.h"

#define E(component, args...) new component(args)

struct Component {
  LayoutContext layout;
  virtual ~Component() {};
  virtual void render(Application *app) = 0;
  virtual void calculateSize() {};
  virtual void updateLayout(LayoutContext &layout) {
    // only update position
    LayoutContext derived = deriveLayout(layout);
    this->layout.position = derived.position;
  };
};

typedef Component *RenderableComponent;

class EmptyComponent : public Component {
public:
  void render(Application *app) override {};
};

#endif // _COMPONENT_TYPES_H_