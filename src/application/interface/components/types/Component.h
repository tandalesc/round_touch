#ifndef _COMPONENT_TYPES_H_
#define _COMPONENT_TYPES_H_

#include "src/application/Application.h"
#include "src/application/interface/layout/LayoutContext.h"
#include "src/events/EventHandler.h"
#include "src/events/types/InputEvent.h"

// helper define so you don't need the "new" keyword everywhere
#define E(component, args...) new component(args)

// unit for efficient rendering and layouts
struct Component : public EventHandler<InputEvent> {
  Application *app;
  // defaults to all zeros
  LayoutContext layout;
  // required for c++ semantics with lifetimes and destructors
  virtual ~Component() {};
  // used to attach application instance to each component when
  // first created as a managed process
  virtual void attachApplication(Application *app);
  // required for layout calculations
  // only needed if this component takes up space in the layout
  // or has children that need their layouts updated
  virtual void calculateSize(LayoutContext &layout) {};
  // required for layout calculations
  // only needed if this component has children that need their
  // layouts updated
  virtual void updateLayout(LayoutContext &layout);
  // required for event handling
  // only needed if this component uses event listeners or has
  // children that need their events handled
  virtual void handleEvent(InputEvent &event) {};
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