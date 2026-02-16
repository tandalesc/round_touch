#include "config/screens/Routes.h"
#include "config/screens/Screens.h"

RenderableComponent createComponentFromState(State state) {
  if (state == NOT_STARTED) {
    return DeviceNotStarted();
  } else if (state == ERROR) {
    return ErrorState();
  } else if (state == READY) {
    return ReadyState();
  } else if (state == ECOMODE) {
    return EcoModeState();
  } else if (state == INFO1) {
    return Info1State();
  } else if (state == INFO2) {
    return Info2State();
  } else if (state == INFO3) {
    return Info3State();
  } else if (state == DETAILS) {
    return DetailsState();
  } else if (state == LIGHTS) {
    return LightsState();
  }
  return E(Component);
}
