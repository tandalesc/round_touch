#include "application/workflow/ComponentLibrary.h"

#include "application/interface/components/types/Layout.h"
#include "application/interface/components/layout/FlexLayout.h"
#include "application/interface/components/input/TouchNavigation.h"
#include "application/interface/components/input/StateChangeRule.h"
#include "application/interface/components/core/FillScreen.h"
#include "application/interface/components/core/Text.h"
#include "application/interface/components/demo/Counter.h"
#include "application/interface/components/ha/HAToggle.h"

// Color constants (hex RGB)
#define CLR_BLACK   0x000000
#define CLR_WHITE   0xFFFFFF
#define CLR_GREEN   0x00FF00

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

RenderableComponent DeviceNotStarted() {
  return E(Component);
}

RenderableComponent ErrorState() {
  return (
    E(FillScreen, {CLR_BLACK},
      E(TouchNavigation, onSwipeDown(READY)),
      E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
        E(Text, "Error!")
      )
    )
  );
}

// READY INFO1 INFO2 INFO3
// ECOMODE

RenderableComponent EcoModeState() {
  return (
    E(FillScreen, {.color = CLR_GREEN},
      E(TouchNavigation, onSwipeUp(READY)),
      E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
        E(Text, {.color = CLR_BLACK}, "ECO Mode")
      )
    )
  );
}

RenderableComponent ReadyState() {
  return (
    E(FillScreen, {CLR_BLACK},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(INFO1),
        onTapAnywhere(DETAILS)
      }),
      E(FlexLayout, {.type=LayoutType::Column, .props = {.gap = 5},},
        E(FlexLayout, {.type=LayoutType::Row, .align=Align::Center},
          E(Text, {.size = 3}, "Home")
        ),
        E(FlexLayout, {.type=LayoutType::Row, .align=Align::Center},
          E(Text, {.size = 2}, "Tap to see more")
        )
      )
    )
  );
}

RenderableComponent Info1State() {
  return (
    E(FillScreen, {CLR_BLACK},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(INFO2),
        onSwipeRight(READY)
      }),
      E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 10}},
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 3}, "Gauges")
        ),
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Counter)
        )
      )
    )
  );
}

RenderableComponent Info2State() {
  return (
    E(FillScreen, {CLR_BLACK},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(INFO3),
        onSwipeRight(INFO1)
      }),
      E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
        E(Text, "Tuning")
      )
    )
  );
}

RenderableComponent Info3State() {
  return (
    E(FillScreen, {CLR_BLACK},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(LIGHTS),
        onSwipeRight(INFO2)
      }),
      E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
        E(Text, "Settings")
      )
    )
  );
}

RenderableComponent DetailsState() {
  return (
    E(FillScreen, {CLR_BLACK},
      E(TouchNavigation, onSwipeUp(READY)),
      E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 8}},
        E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 1}},
          E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
            E(Text, {.size = 3}, "Details")
          )
        ),
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 5}},
            E(Text, {.size = 2}, "2002 Mazda Miata"),
            E(Text, {.size = 2}, "1.8L Stock")
          )
        ),
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 5}},
            E(Text, {.size = 2}, "- Special Edition"),
            E(Text, {.size = 2}, "- 6 Speed"),
            E(Text, {.size = 2}, "- Torsen LSD")
          )
        )
      )
    )
  );
}

RenderableComponent LightsState() {
  return (
    E(FillScreen, {CLR_BLACK},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeRight(INFO3)
      }),
      E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 10}},
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 3}, "Lights")
        ),
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(HAToggle, "light.living_room_lamp")
        )
      )
    )
  );
}
