#include "src/application/workflow/ComponentLibrary.h"

#include "src/application/interface/components/types/Layout.h"
#include "src/application/interface/components/layout/FlexLayout.h"
#include "src/application/interface/components/input/TouchNavigation.h"
#include "src/application/interface/components/input/StateChangeRule.h"
#include "src/application/interface/components/core/FillScreen.h"
#include "src/application/interface/components/core/Text.h"

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
  }
}

RenderableComponent DeviceNotStarted() {
  Serial.println("Device not started.");
  delay(10000);
  return E(Component);
}

RenderableComponent ErrorState() {
  return (
    E(FillScreen, {BLACK},
      E(TouchNavigation, onSwipeDown(READY)),
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Error!")
      )
    )
  );
}

// READY INFO1 INFO2 INFO3
// ECOMODE

RenderableComponent EcoModeState() {
  return (
    E(FillScreen, {.color = GREEN},
      E(TouchNavigation, onSwipeUp(READY)),
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, {.color = BLACK}, "ECO Mode")
      )
    )
  );
}

RenderableComponent ReadyState() {
  return (
    E(FillScreen, {BLACK},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(INFO1),
        onTapAnywhere(DETAILS)
      }),
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Details")
      )
    )
  );
}

RenderableComponent Info1State() {
  return (
    E(FillScreen, {BLACK},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(INFO2),
        onSwipeRight(READY)
      }),
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Gauges")
      )
    )
  );
}

RenderableComponent Info2State() {
  return (
    E(FillScreen, {BLACK},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(INFO3),
        onSwipeRight(INFO1)
      }),
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Tuning")
      )
    )
  );
}

RenderableComponent Info3State() {
  return (
    E(FillScreen, {BLACK},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeRight(INFO2)
      }),
      E(FlexLayout, {.padding = {.t = 20, .l = 20}},
        E(Text, "Settings")
      )
    )
  );
}

RenderableComponent DetailsState() {
  return (
    E(FillScreen, {BLACK},
      E(TouchNavigation, onSwipeUp(READY)),
      E(FlexLayout, {.type = LayoutType::Column},
        E(FlexLayout, {.type = LayoutType::Row, .padding = {.l = 5}},
          E(Text, {.size = 3}, "2002"),
          E(FlexLayout, {.type = LayoutType::Column, .padding = {.l = 5}},
            E(Text, {.size = 2}, "Mazda Miata"),
            E(Text, {.size = 2}, "1.8L Stock")
          )
        ),
        E(FlexLayout, {.type = LayoutType::Column, .padding = {.t = 5, .l = 12}},
          E(Text, {.size = 2}, "Special Edition"),
          E(Text, {.size = 2}, "6 Speed")
        )
      )
    )
  );
}