#ifndef _TOAST_H_
#define _TOAST_H_

#include "lvgl.h"
#include "events/types/InputEvent.h"

// Auto-dismiss duration in milliseconds
#define TOAST_DEFAULT_DURATION_MS 3000

// Toast action callback — called when the action button is tapped.
// The toast is closed automatically after the callback returns.
typedef void (*ToastActionCallback)(void *userData);

struct ToastAction {
  const char *label = nullptr;
  ToastActionCallback callback = nullptr;
  void *userData = nullptr;
};

// Singleton toast notification system.
// Renders on lv_layer_top() so it overlays any screen without
// disrupting the workflow state — like a React context provider.
//
// Input is routed through the custom event system (not LVGL indev),
// so Interface must call Toast::handleEvent() for taps to work.
//
// Usage:
//   Toast::show("WiFi connected");
//   Toast::show("Update available!", {.label = "Update", .callback = onInstall});
//
class Toast {
public:
  // Show a simple auto-dismissing toast.
  static void show(const char *message,
                   uint32_t durationMs = TOAST_DEFAULT_DURATION_MS);

  // Show a toast with an action button and "Later" dismiss button.
  static void show(const char *message, ToastAction action);

  // Dismiss the current toast immediately.
  static void dismiss();

  // Whether a toast is currently visible — use this to suppress
  // other input handling while the overlay is active.
  static bool isVisible();

  // Route a tap event to the toast. Hit-tests tap coordinates
  // against the action/dismiss buttons. Called by Interface before
  // suppressing component input. Returns true if the event was
  // consumed (i.e. toast was visible).
  static bool handleEvent(InputEvent &event);

private:
  static lv_obj_t *_backdrop;
  static lv_obj_t *_card;
  static lv_obj_t *_label;
  static lv_obj_t *_actionBtn;
  static lv_obj_t *_dismissBtn;
  static lv_timer_t *_timer;
  static ToastAction _currentAction;

  static void createCard();
  static void destroy();
  static void timerCb(lv_timer_t *timer);

  // Hit-test: is (x,y) inside the LVGL object's bounding box?
  static bool hitTest(lv_obj_t *obj, int x, int y);
};

#endif // _TOAST_H_
