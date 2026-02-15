#ifndef _COMPONENT_WITH_CHILDREN_H_
#define _COMPONENT_WITH_CHILDREN_H_

#include <vector>

#include "application/interface/components/types/Component.h"
#include "events/types/TouchEvent.h"

// helper implementation to deal with having multiple child components.
// you can pass child renderable components by using parameter expansion
// as used here, or you can write your own implementaiton to use
// initializer lists or directly pass in a vector to populate the
// children list.
// if your component does layout manipulations, then you still
// need to implement custom logic on top of the default layout methods.
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
  // and then call super class definition
  void attachApplication(Application *app) override;
  // by default, just pass calculate size call to all children
  virtual void calculateSize(LayoutContext &layout) override;
  // by default, just pass update layout call to all children with no
  // modifications
  virtual void updateLayout(LayoutContext &layout) override;
  // by default, just pass setup event listeners call to all children
  virtual void handleEvent(InputEvent &event) override;
  // by default, just pass render call to all children
  virtual void render(Application *app) override;
};

#endif // _COMPONENT_WITH_CHILDREN_H_