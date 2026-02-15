#include "application/interface/components/layout/FlexLayout.h"

void FlexLayout::createWidgets(lv_obj_t *parent) {
  lvObj = lv_obj_create(parent);
  lv_obj_remove_style_all(lvObj);
  lv_obj_set_size(lvObj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

  // Set flex direction based on layout type
  if (layout.type == LayoutType::Row) {
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_ROW);
  } else if (layout.type == LayoutType::Column) {
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
  }

  // Set alignment
  lv_flex_align_t mainAlign = LV_FLEX_ALIGN_START;
  lv_flex_align_t crossAlign = LV_FLEX_ALIGN_START;

  if (layout.align == Align::Center) {
    mainAlign = LV_FLEX_ALIGN_CENTER;
    crossAlign = LV_FLEX_ALIGN_CENTER;
  } else if (layout.align == Align::Right) {
    mainAlign = LV_FLEX_ALIGN_END;
  }

  lv_obj_set_flex_align(lvObj, mainAlign, crossAlign, LV_FLEX_ALIGN_CENTER);

  // Set gap
  if (layout.props.gap > 0) {
    lv_obj_set_style_pad_gap(lvObj, layout.props.gap, 0);
  }

  // Create children
  for (auto &child : children) {
    child->createWidgets(lvObj);
  }
}
