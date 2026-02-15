#include "application/interface/components/layout/FlexLayout.h"
#include <math.h>

void FlexLayout::calculateSize(LayoutContext &layout) {
  int width = 0, height = 0;
  LayoutContext derived = deriveLayout(layout);
  this->layout.preferredSize = derived.preferredSize;
  for (auto *child : this->children) {
    child->calculateSize(derived);
    if (this->layout.type == LayoutType::Row) {
      width += child->layout.size.width + this->layout.props.gap;
      // use size instead of preferredSize for row
      // TODO find a better solution
      height = max(height, child->layout.size.height);
    } else if (this->layout.type == LayoutType::Column) {
      width = max(width, child->layout.preferredSize.width);
      height += child->layout.size.height + this->layout.props.gap;
    } else {
      width = max(width, child->layout.preferredSize.width);
      height = max(height, child->layout.preferredSize.height);
    }
  }
  this->layout.size = {width, height};
  if (this->layout.type == LayoutType::Row) {
    this->layout.preferredSize.height = height;
  } else if (this->layout.type == LayoutType::Column) {
    this->layout.preferredSize.width = width;
  } else {
    this->layout.preferredSize = this->layout.size;
  }
}

void FlexLayout::updateLayout(LayoutContext &layout) {
  // update position based on derived position from parent
  LayoutContext derived = deriveLayout(layout);
  this->layout.position = this->initial.position + derived.position;
  this->layout.props.index = derived.props.index;
  // copy context for application to layout elements
  LayoutContext childLayout = this->layout;
  // tweak initial position depending on layout props
  if (this->layout.type == LayoutType::Row ||
      this->layout.type == LayoutType::None) {
    if (this->layout.align == Align::Right) {
      childLayout.position.l += childLayout.preferredSize.width;
    } else if (this->layout.align == Align::Center) {
      childLayout.position.l +=
          childLayout.preferredSize.width / 2 - childLayout.size.width / 2;
    }
  }
  for (int i = 0; i < this->children.size(); i++) {
    auto *child = this->children[i];
    childLayout.props.index = i;
    // update position and pass modified layout context to props
    if (this->layout.type == LayoutType::Row ||
        this->layout.type == LayoutType::None) {
      if (this->layout.align == Align::Left ||
          this->layout.align == Align::Center) {
        child->updateLayout(childLayout);
        childLayout.position.l +=
            child->layout.size.width + this->layout.props.gap;
      } else if (this->layout.align == Align::Right) {
        childLayout.position.l -=
            child->layout.size.width + this->layout.props.gap;
        child->updateLayout(childLayout);
      }
    } else if (this->layout.type == LayoutType::Column) {
      child->updateLayout(childLayout);
      childLayout.position.t +=
          child->layout.size.height + this->layout.props.gap;
    }
  }
}