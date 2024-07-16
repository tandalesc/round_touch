#include <math.h>
#include "src/application/interface/components/layout/FlexLayout.h"

void FlexLayout::calculateSize() {
  int width = 0, height = 0;
  for (auto *child : this->children) {
    child->calculateSize();
    if (this->layout.type == LayoutType::Row) {
      width += child->layout.size.width;
      height = max(height, child->layout.size.height);
    } else if (this->layout.type == LayoutType::Column) {
      width = max(width, child->layout.size.width);
      height += child->layout.size.height;
    } else {
      width = max(width, child->layout.size.width);
      height = max(height, child->layout.size.height);
    }
  }
  this->layout.size = {width, height};
}

void FlexLayout::updateLayout(LayoutContext &layout) {
  // update position based on derived position from parent
  LayoutContext derived = deriveLayout(layout);
  this->layout.position = this->initial.position + derived.position;
  this->layout.props.index = derived.props.index;
  // copy context for application to layout elements
  LayoutContext childLayout = this->layout;
  for (int i = 0; i < this->children.size(); i++) {
    // apply layout props to each child
    auto *child = this->children[i];
    childLayout.props.index = i;
    child->updateLayout(childLayout);
    // update internals to ensure proper placement
    // update list context position
    if (this->layout.type == LayoutType::Row) {
      childLayout.position.l += child->layout.size.width;
    } else {
      childLayout.position.t += child->layout.size.height;
    }
  }
}