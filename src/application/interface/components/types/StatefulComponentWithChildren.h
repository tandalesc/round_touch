#ifndef _STATEFUL_COMPONENT_WITH_CHILDREN_H_
#define _STATEFUL_COMPONENT_WITH_CHILDREN_H_

#include <vector>

#include "application/interface/components/types/StatefulComponent.h"
#include "events/types/TouchEvent.h"

// A stateful component that also manages child components.
// Mirrors ComponentWithChildren but extends StatefulComponent.
struct StatefulComponentWithChildren : public StatefulComponent {
protected:
  std::vector<RenderableComponent> children;

public:
  template <typename... T>
  StatefulComponentWithChildren(T *...children) : children{children...} {};

  ~StatefulComponentWithChildren() {
    for (auto *child : children) {
      delete child;
    }
  }

  void attachApplication(Application *app) override {
    StatefulComponent::attachApplication(app);
    for (auto *child : children) {
      child->attachApplication(app);
    }
  }

  void createWidgets(lv_obj_t *parent) override {
    for (auto *child : children) {
      child->createWidgets(parent);
    }
  }

  void handleEvent(InputEvent &event) override {
    for (auto *child : children) {
      child->handleEvent(event);
    }
  }
};

#endif // _STATEFUL_COMPONENT_WITH_CHILDREN_H_
