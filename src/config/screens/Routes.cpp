#include "config/screens/Routes.h"
#include "config/screens/Screens.h"

#include "application/Application.h"
#include "application/interface/components/core/FillScreen.h"
#include "application/interface/components/input/ScrollContainer.h"
#include "application/interface/components/input/TabBar.h"
#include "application/interface/components/input/TouchNavigation.h"
#include "application/interface/components/input/StateChangeRule.h"

// Wraps JSON-built content in the system shell (nav + tab bar + scroll).
// Tab order and swipe rules are derived from the manifest.
static RenderableComponent wrapUserScreen(State state, Component *content,
                                          Application *app) {
  auto &mgr = app->userScreenManager();
  auto &tabs = mgr.tabs();

  // Build swipe navigation rules from manifest tab order
  std::vector<StateChangeRule> navRules = {onSwipeDown(SYSTEM_SHADE)};
  for (size_t i = 0; i < tabs.size(); i++) {
    if (tabs[i].id != state) continue;
    if (i > 0)              navRules.push_back(onSwipeRight(tabs[i - 1].id));
    if (i < tabs.size() - 1) navRules.push_back(onSwipeLeft(tabs[i + 1].id));
    break;
  }

  // Build TabBar items from manifest tab definitions
  std::vector<TabBarItem> tabItems;
  for (auto &td : tabs) {
    tabItems.push_back({.icon = td.icon.c_str(),
                        .label = td.label.c_str(),
                        .state = td.id});
  }

  return E(FillScreen, {},
    new TouchNavigation(std::move(navRules)),
    new TabBar(state, std::move(tabItems)),
    new ScrollContainer({.maxWidth = 480},
                        std::vector<RenderableComponent>{content})
  );
}

RenderableComponent createComponentFromState(State state, Application *app) {
  // System shell — always compiled, guaranteed to work
  if (state == NOT_STARTED)  return DeviceNotStarted();
  if (state == ERROR)        return ErrorState();
  if (state == SYSTEM_SHADE) return SystemShadeState();

  // User surface — try JSON manifest, fall back to placeholder
  if (app != nullptr && app->userScreenManager().hasScreen(state)) {
    Component *c = app->userScreenManager().buildScreen(state, app->registry());
    if (c != nullptr) return wrapUserScreen(state, c, app);
  }

  return userScreenFallback(state);
}
