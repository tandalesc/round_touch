#include "application/workflow/ComponentLibrary.h"

#include "application/interface/components/types/Layout.h"
#include "application/interface/components/layout/FlexLayout.h"
#include "application/interface/components/input/TouchNavigation.h"
#include "application/interface/components/input/StateChangeRule.h"
#include "application/interface/components/input/NavButton.h"
#include "application/interface/components/input/ScrollContainer.h"
#include "application/interface/components/input/TabBar.h"
#include "application/interface/components/core/FillScreen.h"
#include "application/interface/components/core/Card.h"
#include "application/interface/components/core/Text.h"
#include "application/interface/components/demo/Counter.h"
#include "application/interface/components/ha/HAToggle.h"
#include "application/interface/components/ha/HAWeather.h"
#include "application/interface/components/ha/HABinarySensor.h"
#include "application/interface/components/settings/OTAUpdatePanel.h"
#include "application/interface/components/settings/DisplayInfoPanel.h"
#include "application/interface/components/settings/RotationToggle.h"
#include "config/HAConfig.h"

// Zinc/neutral palette (Tailwind)
#define CLR_ZINC_50   0xFAFAFA
#define CLR_ZINC_100  0xF4F4F5
#define CLR_ZINC_200  0xE4E4E7
#define CLR_ZINC_300  0xD4D4D8
#define CLR_ZINC_400  0xA1A1AA
#define CLR_ZINC_500  0x71717A
#define CLR_ZINC_700  0x3F3F46
#define CLR_ZINC_800  0x27272A
#define CLR_ZINC_900  0x18181B
#define CLR_ZINC_950  0x09090B
// Semantic colors
#define CLR_GREEN     0x22C55E
#define CLR_BLUE      0x3B82F6
#define CLR_RED       0xEF4444
#define CLR_AMBER     0xF59E0B

// Main navigation tabs shared across screens
#define MAIN_TABS \
  {LV_SYMBOL_HOME, "Home", READY}, \
  {LV_SYMBOL_TINT, "Gauges", INFO1}, \
  {LV_SYMBOL_BARS, "Tuning", INFO2}, \
  {LV_SYMBOL_SETTINGS, "Settings", INFO3}, \
  {LV_SYMBOL_DRIVE, "Car", LIGHTS}

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
    E(FillScreen, {.pad = 16, .gap = 10},
      E(TouchNavigation, onSwipeDown(READY)),
      E(Card, {},
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 4, .color = CLR_RED}, "Error!")
        ),
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 2, .color = CLR_ZINC_400}, "Swipe down to dismiss")
        )
      )
    )
  );
}

// READY INFO1 INFO2 INFO3
// ECOMODE

RenderableComponent EcoModeState() {
  return (
    E(FillScreen, {.color = CLR_GREEN, .pad = 16},
      E(TouchNavigation, onSwipeUp(READY)),
      E(Card, {.bg = 0x166534, .border = 0x15803D}, // green-800 / green-700
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 4, .color = CLR_ZINC_50}, "ECO Mode")
        ),
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 2, .color = 0xBBF7D0}, "Low power active") // green-200
        )
      )
    )
  );
}

// Home: Weather, Lights, Presence
RenderableComponent ReadyState() {
  return (
    E(FillScreen, {},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(INFO1)
      }),
      E(TabBar, READY, {MAIN_TABS}),
      E(ScrollContainer, {.maxWidth = 480},
        E(Text, {.size = 4}, LV_SYMBOL_HOME " Home"),
        E(Card, {},
          E(HAWeather, HA_WEATHER_ENTITY)
        ),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, LV_SYMBOL_GPS " Presence"),
            E(HABinarySensor, HA_PRESENCE_1_ENTITY, HA_PRESENCE_1_LABEL),
            E(HABinarySensor, HA_PRESENCE_2_ENTITY, HA_PRESENCE_2_LABEL)
          )
        ),
        E(Text, {.size = 2, .color = CLR_ZINC_400}, LV_SYMBOL_POWER " Lights"),
        E(Card, {.bg = CLR_ZINC_800, .border = CLR_ZINC_700},
          E(HAToggle, HA_LIGHT_1)
        ),
        E(Card, {.bg = CLR_ZINC_800, .border = CLR_ZINC_700},
          E(HAToggle, HA_LIGHT_2)
        ),
        E(Card, {.bg = CLR_ZINC_800, .border = CLR_ZINC_700},
          E(HAToggle, HA_LIGHT_3)
        ),
        E(Card, {.bg = CLR_ZINC_800, .border = CLR_ZINC_700},
          E(HAToggle, HA_LIGHT_4)
        )
      )
    )
  );
}

RenderableComponent Info1State() {
  return (
    E(FillScreen, {},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(INFO2),
        onSwipeRight(READY)
      }),
      E(TabBar, INFO1, {MAIN_TABS}),
      E(ScrollContainer, {.maxWidth = 480},
        E(Text, {.size = 4}, LV_SYMBOL_TINT " Gauges"),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
            E(Counter)
          )
        ),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, "Coolant"),
            E(Text, {}, "185 F")
          )
        ),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, "Oil Pressure"),
            E(Text, {}, "42 PSI")
          )
        )
      )
    )
  );
}

RenderableComponent Info2State() {
  return (
    E(FillScreen, {},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(INFO3),
        onSwipeRight(INFO1)
      }),
      E(TabBar, INFO2, {MAIN_TABS}),
      E(ScrollContainer, {.maxWidth = 480},
        E(Text, {.size = 4}, LV_SYMBOL_BARS " Tuning"),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, "AFR Target"),
            E(Text, {}, "14.7:1")
          )
        ),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, "Timing Advance"),
            E(Text, {}, "14 deg")
          )
        ),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, "Boost"),
            E(Text, {}, "0.0 PSI")
          )
        )
      )
    )
  );
}

RenderableComponent Info3State() {
  return (
    E(FillScreen, {},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeLeft(LIGHTS),
        onSwipeRight(INFO2)
      }),
      E(TabBar, INFO3, {MAIN_TABS}),
      E(ScrollContainer, {.maxWidth = 480},
        E(Text, {.size = 4}, LV_SYMBOL_SETTINGS " Settings"),
        E(Card, {},
          E(OTAUpdatePanel)
        ),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, LV_SYMBOL_WIFI " WiFi"),
            E(Text, {.color = CLR_GREEN}, "Connected")
          )
        ),
        E(Card, {},
          E(DisplayInfoPanel),
          E(RotationToggle)
        )
      )
    )
  );
}

// Details sub-page (navigated from Car tab)
RenderableComponent DetailsState() {
  return (
    E(FillScreen, {},
      E(ScrollContainer, {.maxWidth = 480},
        E(NavButton, LV_SYMBOL_LEFT " Back", LIGHTS, CLR_ZINC_800, CLR_ZINC_300),
        E(Text, {.size = 4}, "Vehicle Details"),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, "Vehicle"),
            E(Text, {}, "2002 Mazda Miata"),
            E(Text, {.size = 2, .color = CLR_ZINC_500}, "1.8L Stock")
          )
        ),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, "Options"),
            E(Text, {}, "Special Edition"),
            E(Text, {}, "6 Speed"),
            E(Text, {}, "Torsen LSD")
          )
        ),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, "Mileage"),
            E(Text, {}, "127,432 mi")
          )
        )
      )
    )
  );
}

// Car tab (was Lights) — vehicle overview + link to details
RenderableComponent LightsState() {
  return (
    E(FillScreen, {},
      E(TouchNavigation, {
        onSwipeDown(ECOMODE),
        onSwipeRight(INFO3)
      }),
      E(TabBar, LIGHTS, {MAIN_TABS}),
      E(ScrollContainer, {.maxWidth = 480},
        E(Text, {.size = 4}, LV_SYMBOL_DRIVE " Car"),
        E(Card, {},
          E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 4}},
            E(Text, {.size = 2, .color = CLR_ZINC_400}, "Vehicle"),
            E(Text, {}, "2002 Mazda Miata"),
            E(Text, {.size = 2, .color = CLR_ZINC_500}, "1.8L Stock")
          )
        ),
        E(NavButton, LV_SYMBOL_RIGHT " Vehicle Details", DETAILS,
            CLR_ZINC_800, CLR_ZINC_300)
      )
    )
  );
}
