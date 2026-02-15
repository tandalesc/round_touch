#ifndef _COMPONENT_WITH_CHILDREN_H_
#define _COMPONENT_WITH_CHILDREN_H_

#include <vector>

#include "application/interface/components/types/Component.h"
#include "events/types/TouchEvent.h"

struct ComponentWithChildren : public Component {
protected:
  std::vector<RenderableComponent> children;

public:
  // use parameter expansion to populate children vector
  template <typename... T>
  ComponentWithChildren(T *...children) : children{children...} {};
  // if the parent is being deleted, then so are all of its children
  ~ComponentWithChildren();
  // by default, pass attach application call to all children
  void attachApplication(Application *app) override;
  // creates LVGL widgets for this component and all children
  void createWidgets(lv_obj_t *parent) override;
  // by default, just pass event handling to all children
  virtual void handleEvent(InputEvent &event) override;
};

#endif // _COMPONENT_WITH_CHILDREN_H_
