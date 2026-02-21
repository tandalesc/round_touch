#include "config/screens/Screens.h"
#include "config/screens/Theme.h"
#include "config/screens/TitledCard.h"

#include "application/interface/components/layout/FlexLayout.h"
#include "application/interface/components/input/TouchNavigation.h"
#include "application/interface/components/input/StateChangeRule.h"
#include "application/interface/components/input/ScrollContainer.h"
#include "application/interface/components/core/FillScreen.h"
#include "application/interface/components/core/Card.h"
#include "application/interface/components/core/Text.h"
#include "application/interface/components/settings/OTAUpdatePanel.h"
#include "application/interface/components/settings/DisplayInfoPanel.h"
#include "application/interface/components/settings/ContentRefreshPanel.h"
#include "application/interface/components/settings/RotationToggle.h"

// ---------------------------------------------------------------------------
// System screens (compiled, always available)
// ---------------------------------------------------------------------------

RenderableComponent DeviceNotStarted() {
  return E(Component);
}

RenderableComponent ErrorState() {
  return (
    E(FillScreen, {.pad = 16, .gap = 10},
      E(TouchNavigation, onSwipeDown(GO_BACK)),
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

RenderableComponent SystemShadeState() {
  return (
    E(FillScreen, {},
      E(TouchNavigation, onSwipeUp(GO_BACK)),
      E(ScrollContainer, {.maxWidth = 480},
        E(Text, {.size = 4}, LV_SYMBOL_SETTINGS " System"),
        E(Card, {},
          E(OTAUpdatePanel)
        ),
        E(TitledCard, {.icon = LV_SYMBOL_WIFI, .title = "WiFi"},
          E(Text, {.color = CLR_GREEN}, "Connected")
        ),
        E(Card, {},
          E(ContentRefreshPanel)
        ),
        E(Card, {},
          E(DisplayInfoPanel),
          E(RotationToggle)
        )
      )
    )
  );
}

// ---------------------------------------------------------------------------
// User screen fallback (shown when no JSON manifest is loaded)
// ---------------------------------------------------------------------------

RenderableComponent userScreenFallback(State state) {
  return (
    E(FillScreen, {.pad = 16, .gap = 10},
      E(TouchNavigation, onSwipeDown(SYSTEM_SHADE)),
      E(Card, {},
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 4}, LV_SYMBOL_WARNING " No Dashboard")
        ),
        E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
          E(Text, {.size = 2, .color = CLR_ZINC_400},
            "Connect to the control server\nto load your dashboard.")
        )
      )
    )
  );
}
