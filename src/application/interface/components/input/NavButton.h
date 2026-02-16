#ifndef _NAV_BUTTON_H_
#define _NAV_BUTTON_H_

#include "application/interface/components/input/Button.h"
#include "application/workflow/Workflow.h"

// Navigation button — a styled Button that navigates to a target state.
// Uses Button's built-in hit-testing and debounce.
class NavButton : public Button {
  static void navigateCb(Application *app, void *userData) {
    auto targetState = static_cast<State>(reinterpret_cast<intptr_t>(userData));
    app->workflow().navigate(targetState);
  }

public:
  NavButton(const char *label, State targetState,
            uint32_t bg = 0x27272A, uint32_t color = 0xFAFAFA)
      : Button({.label = label, .bg = bg, .color = color},
               navigateCb,
               reinterpret_cast<void *>(static_cast<intptr_t>(targetState))) {}
};

#endif // _NAV_BUTTON_H_
