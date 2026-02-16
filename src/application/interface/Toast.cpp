#include "application/interface/Toast.h"
#include "events/types/TouchEvent.h"

lv_obj_t *Toast::_backdrop = nullptr;
lv_obj_t *Toast::_card = nullptr;
lv_obj_t *Toast::_label = nullptr;
lv_obj_t *Toast::_actionBtn = nullptr;
lv_obj_t *Toast::_dismissBtn = nullptr;
lv_timer_t *Toast::_timer = nullptr;
ToastAction Toast::_currentAction = {};

void Toast::show(const char *message, uint32_t durationMs) {
  destroy();
  createCard();

  lv_label_set_text(_label, message);

  _timer = lv_timer_create(timerCb, durationMs, nullptr);
  lv_timer_set_repeat_count(_timer, 1);
}

void Toast::show(const char *message, ToastAction action) {
  destroy();
  createCard();

  lv_label_set_text(_label, message);
  _currentAction = action;

  // Button row — horizontal flex container for action + dismiss
  lv_obj_t *row = lv_obj_create(_card);
  lv_obj_remove_style_all(row);
  lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_column(row, 10, 0);

  if (action.label != nullptr) {
    // Primary action button (blue)
    _actionBtn = lv_obj_create(row);
    lv_obj_remove_style_all(_actionBtn);
    lv_obj_set_size(_actionBtn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(_actionBtn, lv_color_hex(0x3B82F6), 0); // blue-500
    lv_obj_set_style_bg_opa(_actionBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(_actionBtn, 6, 0);
    lv_obj_set_style_pad_hor(_actionBtn, 16, 0);
    lv_obj_set_style_pad_ver(_actionBtn, 8, 0);

    lv_obj_t *btnLabel = lv_label_create(_actionBtn);
    lv_label_set_text(btnLabel, action.label);
    lv_obj_set_style_text_color(btnLabel, lv_color_hex(0xFAFAFA), 0); // zinc-50
  }

  // "Later" dismiss button (subtle gray)
  _dismissBtn = lv_obj_create(row);
  lv_obj_remove_style_all(_dismissBtn);
  lv_obj_set_size(_dismissBtn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_color(_dismissBtn, lv_color_hex(0x27272A), 0); // zinc-800
  lv_obj_set_style_bg_opa(_dismissBtn, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(_dismissBtn, 6, 0);
  lv_obj_set_style_pad_hor(_dismissBtn, 16, 0);
  lv_obj_set_style_pad_ver(_dismissBtn, 8, 0);

  lv_obj_t *dismissLabel = lv_label_create(_dismissBtn);
  lv_label_set_text(dismissLabel, "Later");
  lv_obj_set_style_text_color(dismissLabel, lv_color_hex(0xA1A1AA), 0); // zinc-400
}

void Toast::dismiss() { destroy(); }

bool Toast::isVisible() { return _backdrop != nullptr; }

bool Toast::handleEvent(InputEvent &event) {
  if (!isVisible()) return false;

  // Only handle tap events
  if (event.inputType != InputType::TouchInput) return true;
  auto *touch = static_cast<TouchEvent *>(&event);
  if (touch->type != TouchType::TapType) return true;

  auto *tap = static_cast<TapTouchEvent *>(touch);
  int x = tap->location.x;
  int y = tap->location.y;

  // Check action button first
  if (_actionBtn != nullptr && hitTest(_actionBtn, x, y)) {
    ToastAction action = _currentAction;
    destroy();
    if (action.callback != nullptr) {
      action.callback(action.userData);
    }
    return true;
  }

  // Check dismiss button
  if (_dismissBtn != nullptr && hitTest(_dismissBtn, x, y)) {
    destroy();
    return true;
  }

  // Tap anywhere else on the backdrop dismisses (but not on the card body)
  if (!hitTest(_card, x, y)) {
    destroy();
  }

  // Always consume the event while toast is visible
  return true;
}

void Toast::createCard() {
  lv_obj_t *layer = lv_layer_top();

  // Semi-transparent backdrop — covers full screen
  _backdrop = lv_obj_create(layer);
  lv_obj_remove_style_all(_backdrop);
  lv_obj_set_size(_backdrop, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(_backdrop, lv_color_hex(0x09090B), 0); // zinc-950
  lv_obj_set_style_bg_opa(_backdrop, LV_OPA_50, 0);

  // Card overlay — centered, dark bg, rounded corners, padded.
  // Uses percentage width so it scales across 240px and 800px screens.
  _card = lv_obj_create(_backdrop);
  lv_obj_remove_style_all(_card);
  lv_obj_set_size(_card, LV_PCT(80), LV_SIZE_CONTENT);
  lv_obj_align(_card, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_bg_color(_card, lv_color_hex(0x18181B), 0); // zinc-900
  lv_obj_set_style_bg_opa(_card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(_card, lv_color_hex(0x3F3F46), 0); // zinc-700
  lv_obj_set_style_border_width(_card, 1, 0);
  lv_obj_set_style_border_opa(_card, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(_card, 12, 0);
  lv_obj_set_style_pad_all(_card, 16, 0);
  lv_obj_set_flex_flow(_card, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(_card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(_card, 12, 0);

  // Message label — wraps text, centered
  _label = lv_label_create(_card);
  lv_obj_set_width(_label, LV_PCT(100));
  lv_label_set_long_mode(_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_color(_label, lv_color_hex(0xFAFAFA), 0); // zinc-50
  lv_obj_set_style_text_align(_label, LV_TEXT_ALIGN_CENTER, 0);
}

void Toast::destroy() {
  if (_timer != nullptr) {
    lv_timer_delete(_timer);
    _timer = nullptr;
  }
  if (_backdrop != nullptr) {
    lv_obj_delete(_backdrop);
    _backdrop = nullptr;
    _card = nullptr;
    _label = nullptr;
    _actionBtn = nullptr;
    _dismissBtn = nullptr;
  }
  _currentAction = {};
}

void Toast::timerCb(lv_timer_t *timer) {
  (void)timer;
  destroy();
}

bool Toast::hitTest(lv_obj_t *obj, int x, int y) {
  if (obj == nullptr) return false;
  lv_area_t area;
  lv_obj_get_coords(obj, &area);
  return x >= area.x1 && x <= area.x2 && y >= area.y1 && y <= area.y2;
}
