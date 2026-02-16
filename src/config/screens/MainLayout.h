#ifndef _MAIN_LAYOUT_H_
#define _MAIN_LAYOUT_H_

#include <vector>

#include "application/interface/components/core/FillScreen.h"
#include "application/interface/components/input/TouchNavigation.h"
#include "application/interface/components/input/StateChangeRule.h"
#include "application/interface/components/input/ScrollContainer.h"
#include "application/interface/components/input/TabBar.h"
#include "config/screens/Theme.h"

// Tab order — must match MAIN_TABS
static constexpr State TAB_ORDER[] = {READY, INFO1, INFO2, INFO3, LIGHTS};
static constexpr int TAB_COUNT = sizeof(TAB_ORDER) / sizeof(TAB_ORDER[0]);

// Shared layout for all main tabbed screens:
//   FillScreen > TouchNavigation + TabBar + ScrollContainer(children...)
//
// Swipe navigation is derived automatically from the tab's position:
//   - swipe down → ECOMODE (always)
//   - swipe left → next tab (if not last)
//   - swipe right → prev tab (if not first)
//
// Usage:
//   MainLayout(READY,
//     E(Text, {.size = 4}, "Home"),
//     E(Card, {}, ...)
//   )
template <typename... T>
RenderableComponent MainLayout(State activeTab, T *...children) {
  std::vector<StateChangeRule> navRules = {onSwipeDown(ECOMODE)};
  for (int i = 0; i < TAB_COUNT; i++) {
    if (TAB_ORDER[i] != activeTab) continue;
    if (i > 0)
      navRules.push_back(onSwipeRight(TAB_ORDER[i - 1]));
    if (i < TAB_COUNT - 1)
      navRules.push_back(onSwipeLeft(TAB_ORDER[i + 1]));
    break;
  }

  return E(FillScreen, {},
    new TouchNavigation(std::move(navRules)),
    E(TabBar, activeTab, {MAIN_TABS}),
    E(ScrollContainer, {.maxWidth = 480}, children...)
  );
}

#endif // _MAIN_LAYOUT_H_
