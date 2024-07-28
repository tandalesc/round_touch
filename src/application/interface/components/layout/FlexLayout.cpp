#include "src/application/interface/components/layout/FlexLayout.h"
#include <math.h>

void FlexLayout::calculateSize(LayoutContext &layout) {
  ComponentWithChildren::calculateSize(layout);
  // calculate new size by visiting all children
  Size newSize;
  for (auto *child : this->children) {
    Size childSize = child->layout.props.size;
    Size childPreferredSize = child->layout.props.size;
    if (this->layout.type == LayoutType::Row) {
      newSize.width += childSize.width + this->layout.gap;
      // use size instead of preferredSize for row
      // TODO find a better solution
      newSize.height = max(newSize.height, childSize.height);
    } else if (this->layout.type == LayoutType::Column) {
      newSize.width = max(newSize.width, childPreferredSize.width);
      newSize.height += childSize.height + this->layout.gap;
    } else {
      newSize.width = max(newSize.width, childPreferredSize.width);
      newSize.height = max(newSize.height, childPreferredSize.height);
    }
  }
  // update size and preferred size
  this->layout.props.size = newSize;
  this->layout.props.preferredSize =
      layout.props.preferredSize - layout.padding;
  if (this->layout.type == LayoutType::Row) {
    this->layout.props.preferredSize.height = newSize.height;
  } else if (this->layout.type == LayoutType::Column) {
    this->layout.props.preferredSize.width = newSize.width;
  } else {
    this->layout.props.preferredSize = newSize;
  }
}

void FlexLayout::updateLayout(LayoutContext &layout) {
  // update position based on derived position from parent
  LayoutContext derived = deriveLayout(layout);
  this->layout.props.position =
      this->initial.props.position + derived.props.position;
  this->layout.props.index = derived.props.index;
  // copy context for application to layout elements
  LayoutContext &childContext = this->layout;
  ContainerProps &childProps = childContext.props;
  // tweak initial position depending on layout props
  if (this->layout.type == LayoutType::Row ||
      this->layout.type == LayoutType::None) {
    if (this->layout.align == Align::Right) {
      childProps.position.l += childProps.preferredSize.width;
    } else if (this->layout.align == Align::Center) {
      childProps.position.l +=
          childProps.preferredSize.width / 2 - childProps.size.width / 2;
    }
  }
  for (int i = 0; i < this->children.size(); i++) {
    auto *child = this->children[i];
    childProps.index = i;
    // update position and pass modified layout context to props
    if (this->layout.type == LayoutType::Row ||
        this->layout.type == LayoutType::None) {
      if (this->layout.align == Align::Left ||
          this->layout.align == Align::Center) {
        child->updateLayout(childContext);
        childProps.position.l +=
            child->layout.props.size.width + this->layout.gap;
      } else if (this->layout.align == Align::Right) {
        childProps.position.l -=
            child->layout.props.size.width + this->layout.gap;
        child->updateLayout(childContext);
      }
    } else if (this->layout.type == LayoutType::Column) {
      child->updateLayout(childContext);
      childProps.position.t +=
          child->layout.props.size.height + this->layout.gap;
    }
  }
}