#ifndef _COMPONENT_TYPES_H_
#define _COMPONENT_TYPES_H_

#include "lvgl.h"

#include "application/interface/components/types/Layout.h"
#include "events/EventHandler.h"
#include "events/types/InputEvent.h"

// forward declaration
class Application;

// helper define so you don't need the "new" keyword everywhere
#define E(component, args...) new component(args)

// unit for efficient rendering and layouts
struct Component : public EventHandler<InputEvent> {
  Application *app = nullptr;
  // the LVGL object this component manages
  lv_obj_t *lvObj = nullptr;
  // defaults to all zeros
  LayoutContext layout;
  // required for c++ semantics with lifetimes and destructors
  virtual ~Component();
  // used to attach application instance to each component when
  // first created as a managed process
  virtual void attachApplication(Application *app);
  // creates the LVGL widget tree under the given parent
  virtual void createWidgets(lv_obj_t *parent) {};
  // required for event handling
  // only needed if this component uses event listeners or has
  // children that need their events handled
  virtual void handleEvent(InputEvent &event) {};
};

// convinience helper to keep track of which components
// are ready to render and which ones still need
// instantiation.
typedef Component *RenderableComponent;

#endif // _COMPONENT_TYPES_H_
