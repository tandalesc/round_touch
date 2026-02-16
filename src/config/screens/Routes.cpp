#include "config/screens/Routes.h"
#include "config/screens/Screens.h"

RenderableComponent createComponentFromState(State state, Application *app) {
  // System shell — always compiled, guaranteed to work
  if (state == NOT_STARTED)  return DeviceNotStarted();
  if (state == ERROR)        return ErrorState();
  if (state == SYSTEM_SHADE) return SystemShadeState();

  // User surface — JSON pipeline will be wired here in a later commit.
  // For now, all user states get the fallback placeholder.
  return userScreenFallback(state);
}
