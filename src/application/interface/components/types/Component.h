#ifndef _COMPONENT_TYPES_H_
#define _COMPONENT_TYPES_H_

#include "src/application/Application.h"
#include "src/application/interface/components/types/Layout.h"

// helper define so you don't need the "new" keyword everywhere
#define E(component, args...) new component(args)

// unit for efficient rendering and layouts
struct Component {
  // defaults to all zeros
  LayoutContext layout;
  // required for c++ semantics with lifetimes and destructors
  virtual ~Component() {};
  // required for layout calculations
  // only needed if this component takes up space in the layout
  // or has children that need their layouts updated
  virtual void calculateSize() {};
  // required for layout calculations
  // only needed if this component has children that need their
  // layouts updated
  virtual void updateLayout(LayoutContext &layout);
  // required for event handling
  // only needed if this component uses event listeners or has
  // children that need their events handled
  virtual void setupEventListeners(Application *app) {};
  // required for rendering
  virtual void render(Application *app) {};
  // convinience helper function for layout purposes
  void drawDebugBounds(Application *app);
};

// convinience helper to keep track of which components
// are ready to render and which ones still need
// instantiation.
typedef Component *RenderableComponent;

#endif // _COMPONENT_TYPES_H_