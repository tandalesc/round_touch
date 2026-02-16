#include "config/screens/Screens.h"
#include "config/screens/Theme.h"
#include "config/screens/MainLayout.h"
#include "config/screens/TitledCard.h"
#include "config/screens/GaugeCard.h"

#include "application/interface/components/types/Layout.h"
#include "application/interface/components/layout/FlexLayout.h"
#include "application/interface/components/input/TouchNavigation.h"
#include "application/interface/components/input/StateChangeRule.h"
#include "application/interface/components/input/NavButton.h"
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

// ---------------------------------------------------------------------------
// Reusable subtree components
// ---------------------------------------------------------------------------

RenderableComponent PresenceSection() {
  return E(TitledCard, {.icon = LV_SYMBOL_GPS, .title = "Presence"},
    E(HABinarySensor, HA_PRESENCE_1_ENTITY, HA_PRESENCE_1_LABEL),
    E(HABinarySensor, HA_PRESENCE_2_ENTITY, HA_PRESENCE_2_LABEL)
  );
}

RenderableComponent LightSection() {
  return E(FlexLayout, {.type = LayoutType::Column, .props = {.gap = 8}},
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
  );
}

// ---------------------------------------------------------------------------
// Screen definitions
// ---------------------------------------------------------------------------

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

RenderableComponent EcoModeState() {
  return (
    E(FillScreen, {.color = CLR_GREEN, .pad = 16},
      E(TouchNavigation, onSwipeUp(READY)),
      E(Card, {.bg = CLR_GREEN_800, .border = CLR_GREEN_700},
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 4, .color = CLR_ZINC_50}, "ECO Mode")
        ),
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 2, .color = CLR_GREEN_200}, "Low power active")
        )
      )
    )
  );
}

RenderableComponent ReadyState() {
  return MainLayout(READY,
    E(Text, {.size = 4}, LV_SYMBOL_HOME " Home"),
    E(Card, {},
      E(HAWeather, HA_WEATHER_ENTITY)
    ),
    PresenceSection(),
    LightSection()
  );
}

RenderableComponent Info1State() {
  return MainLayout(INFO1,
    E(Text, {.size = 4}, LV_SYMBOL_TINT " Gauges"),
    E(Card, {},
      E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
        E(Counter)
      )
    ),
    E(GaugeCard, {.label = "Coolant", .value = "185 F"}),
    E(GaugeCard, {.label = "Oil Pressure", .value = "42 PSI"})
  );
}

RenderableComponent Info2State() {
  return MainLayout(INFO2,
    E(Text, {.size = 4}, LV_SYMBOL_BARS " Tuning"),
    E(GaugeCard, {.label = "AFR Target", .value = "14.7:1"}),
    E(GaugeCard, {.label = "Timing Advance", .value = "14 deg"}),
    E(GaugeCard, {.label = "Boost", .value = "0.0 PSI"})
  );
}

RenderableComponent Info3State() {
  return MainLayout(INFO3,
    E(Text, {.size = 4}, LV_SYMBOL_SETTINGS " Settings"),
    E(Card, {},
      E(OTAUpdatePanel)
    ),
    E(TitledCard, {.icon = LV_SYMBOL_WIFI, .title = "WiFi"},
      E(Text, {.color = CLR_GREEN}, "Connected")
    ),
    E(Card, {},
      E(DisplayInfoPanel),
      E(RotationToggle)
    )
  );
}

RenderableComponent DetailsState() {
  return (
    E(FillScreen, {},
      E(ScrollContainer, {.maxWidth = 480},
        E(NavButton, LV_SYMBOL_LEFT " Back", LIGHTS, CLR_ZINC_800, CLR_ZINC_300),
        E(Text, {.size = 4}, "Vehicle Details"),
        E(TitledCard, {.title = "Vehicle"},
          E(Text, {}, "2002 Mazda Miata"),
          E(Text, {.size = 2, .color = CLR_ZINC_500}, "1.8L Stock")
        ),
        E(TitledCard, {.title = "Options"},
          E(Text, {}, "Special Edition"),
          E(Text, {}, "6 Speed"),
          E(Text, {}, "Torsen LSD")
        ),
        E(GaugeCard, {.label = "Mileage", .value = "127,432 mi"})
      )
    )
  );
}

RenderableComponent LightsState() {
  return MainLayout(LIGHTS,
    E(Text, {.size = 4}, LV_SYMBOL_DRIVE " Car"),
    E(TitledCard, {.title = "Vehicle"},
      E(Text, {}, "2002 Mazda Miata"),
      E(Text, {.size = 2, .color = CLR_ZINC_500}, "1.8L Stock")
    ),
    E(NavButton, LV_SYMBOL_RIGHT " Vehicle Details", DETAILS,
        CLR_ZINC_800, CLR_ZINC_300)
  );
}
