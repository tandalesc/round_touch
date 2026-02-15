#ifndef _STATEFUL_COMPONENT_H_
#define _STATEFUL_COMPONENT_H_

#include "application/interface/components/types/Component.h"

// A component that can update its LVGL widgets in-place after creation.
// Subclasses store state as member variables, mutate them in event handlers
// or callbacks, then call update() to sync the LVGL widgets.
struct StatefulComponent : public Component {
  // Override to sync LVGL widgets with current state.
  // Called by subclasses after mutating their state.
  virtual void update() {};
};

#endif // _STATEFUL_COMPONENT_H_
