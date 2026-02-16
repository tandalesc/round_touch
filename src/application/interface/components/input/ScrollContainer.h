#ifndef _SCROLL_CONTAINER_H_
#define _SCROLL_CONTAINER_H_

#include "application/Application.h"
#include "application/interface/components/types/ComponentWithChildren.h"
#include "events/types/TouchEvent.h"

// Scroll step scales with screen height — small on 240px, large on 480px
#define SCROLL_STEP_SMALL 80
#define SCROLL_STEP_LARGE 200

struct ScrollContainerProps {
  int pad = 16;
  int gap = 8;
  int maxWidth = 0; // 0 = no limit
};

class ScrollContainer : public ComponentWithChildren {
private:
  ScrollContainerProps props;

public:
  template <typename... T>
  ScrollContainer(ScrollContainerProps props, T *...children)
      : ComponentWithChildren(children...), props(props) {};

  template <typename... T>
  ScrollContainer(T *...children) : ScrollContainer({}, children...) {};

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);

    // Compute width: use maxWidth if set and smaller than screen
    int screenW = (app != nullptr) ? app->device()->display().width() : 0;
    if (props.maxWidth > 0 && screenW > props.maxWidth) {
      lv_obj_set_width(lvObj, props.maxWidth);
    } else {
      lv_obj_set_width(lvObj, LV_PCT(100));
    }
    lv_obj_set_height(lvObj, LV_PCT(100));
    lv_obj_set_flex_grow(lvObj, 1);

    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    // Padding — extra top/bottom for circular displays
    int padV = props.pad;
    if (app != nullptr && app->device()->display().isCircular()) {
      int h = app->device()->display().height();
      padV = h * 18 / 100; // ~43px on 240px display
    }
    lv_obj_set_style_pad_left(lvObj, props.pad, 0);
    lv_obj_set_style_pad_right(lvObj, props.pad, 0);
    lv_obj_set_style_pad_top(lvObj, padV, 0);
    lv_obj_set_style_pad_bottom(lvObj, padV, 0);
    lv_obj_set_style_pad_row(lvObj, props.gap, 0);

    lv_obj_add_flag(lvObj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(lvObj, LV_SCROLLBAR_MODE_OFF);

    for (auto &child : children) {
      child->createWidgets(lvObj);
    }
  }

  void handleEvent(InputEvent &event) override {
    // Forward to children first
    ComponentWithChildren::handleEvent(event);

    if (lvObj == nullptr) return;
    if (event.inputType != InputType::TouchInput) return;
    auto &te = static_cast<TouchEvent &>(event);
    if (te.type != TouchType::SwipeType) return;

    auto &se = static_cast<SwipeTouchEvent &>(event);
    bool isVertical = se.direction == SwipeDirection::SwipeUp ||
                      se.direction == SwipeDirection::SwipeDown;
    if (!isVertical) return;

    // Only scroll for non-edge swipes — edge swipes are reserved for
    // screen navigation (handled by TouchNavigation)
    int h = lv_obj_get_height(lvObj);
    int edgeZone = h * 15 / 100;
    int y = se.startLocation.y;
    bool fromEdge = (y < edgeZone) || (y > h - edgeZone);
    if (fromEdge) return;

    // Clamp: don't scroll past top or bottom
    int scrollY = lv_obj_get_scroll_y(lvObj);
    int scrollBottom = lv_obj_get_scroll_bottom(lvObj);

    // Scale scroll step and animation to screen size
    bool isLarge = h > 300;
    int step = isLarge ? SCROLL_STEP_LARGE : SCROLL_STEP_SMALL;
    lv_anim_enable_t anim = isLarge ? LV_ANIM_OFF : LV_ANIM_ON;

    if (se.direction == SwipeDirection::SwipeUp) {
      // Swipe up = scroll content down (reveal more below)
      if (scrollBottom <= 0) return; // already at bottom
      int dy = -step;
      if (-dy > scrollBottom) dy = -scrollBottom;
      lv_obj_scroll_by(lvObj, 0, dy, anim);
    } else {
      // Swipe down = scroll content up (reveal more above)
      if (scrollY <= 0) return; // already at top
      int dy = step;
      if (dy > scrollY) dy = scrollY;
      lv_obj_scroll_by(lvObj, 0, dy, anim);
    }
  }
};

#endif // _SCROLL_CONTAINER_H_
