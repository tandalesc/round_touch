#include "src/application/interface/components/layout/FlexLayout.h"
#include <math.h>

void FlexLayout::calculateSize(LayoutContext &layout) {
  LayoutContext derived = deriveLayout(layout);
  // calculate new size by visiting all children
  Size sumSize;
  Size largestSize;
  bool overflow = false;
  for (auto *child : this->children) {
    child->calculateSize(derived);
    Size childSize = child->layout.props.size;
    Size childPreferredSize = child->layout.props.preferredSize;
    sumSize.width += childSize.width + this->layout.gap;
    sumSize.height += childSize.height + this->layout.gap;
    largestSize.width = max(largestSize.width, childSize.width);
    largestSize.height = max(largestSize.height, childSize.height);
  }
  Size newSize;
  if(this->layout.type == LayoutType::Row) {
    if(this->layout.grow == 0) {
      newSize.width = sumSize.width;
    } else {
      newSize.width = derived.props.preferredSize.width;
    }
    newSize.height = largestSize.height;
  } else if (this->layout.type == LayoutType::Column) {
    if(this->layout.grow == 0) {
      newSize.height = sumSize.height;
    } else {
      newSize.height = derived.props.preferredSize.height;
    }
    newSize.width = largestSize.width;
  }
  // update size and preferred size
  this->layout.props.size = newSize;
  this->layout.props.preferredSize = derived.props.preferredSize;

  if (this->layout.type == LayoutType::None) {
    this->layout.props.preferredSize = newSize;
  }
}

void FlexLayout::updateLayout(LayoutContext &layout) {
  LayoutContext &myLayout = this->layout;
  // update position based on derived position from parent
  LayoutContext derived = deriveLayout(layout);
  myLayout.props.position =
      this->initial.props.position + derived.props.position;
  myLayout.props.index = derived.props.index;
  // copy context for application to layout elements
  LayoutContext childCtx = LayoutContext(myLayout);
  ContainerProps initialCtxProps = ContainerProps(childCtx.props);
  // initialize child ctx position depending on layout props
  if (myLayout.type == LayoutType::Row) {
    if (myLayout.major == Align::End) {
      initialCtxProps.position.l += initialCtxProps.preferredSize.width;
    } else if (myLayout.major == Align::Center) {
      initialCtxProps.position.l += initialCtxProps.preferredSize.width / 2 -
                                    initialCtxProps.size.width / 2;
    }
    if (myLayout.minor == Align::End) {
      initialCtxProps.position.t += initialCtxProps.preferredSize.height;
    }
  } else if (myLayout.type == LayoutType::Column) {
    if (myLayout.major == Align::End) {
      initialCtxProps.position.t += initialCtxProps.preferredSize.height;
    } else if (myLayout.major == Align::Center) {
      initialCtxProps.position.t += initialCtxProps.preferredSize.height / 2 -
                                    initialCtxProps.size.height / 2;
    }
    if (myLayout.minor == Align::End) {
      initialCtxProps.position.l += initialCtxProps.preferredSize.width;
    }
  }
  childCtx.props.position = initialCtxProps.position;
  ContainerProps &ctxProps = childCtx.props;
  bool overflow = false;
  for (int i = 0; i < this->children.size(); i++) {
    auto *child = this->children[i];
    ContainerProps &childProps = child->layout.props;
    // update list position index
    ctxProps.index = i;
    // ctxProps.position = initialCtxProps.position;
    bool firstElement = i == 0;
    bool allowMajorLeadingUpdates = !firstElement;
    // update position and pass modified layout context to props
    if (myLayout.type == LayoutType::Row) {
      if (myLayout.major == Align::Start || myLayout.major == Align::Center) {
        if (allowMajorLeadingUpdates) {
          ctxProps.position.l += childProps.size.width + myLayout.gap;
        }
      } else if (myLayout.major == Align::End) {
        ctxProps.position.l -= childProps.size.width + myLayout.gap;
      }
      if (myLayout.minor == Align::Center) {
        ctxProps.position.t =
            initialCtxProps.position.t +
            (ctxProps.preferredSize.height / 2 - childProps.size.height / 2);
      } else if (myLayout.minor == Align::End) {
        ctxProps.position.t =
            initialCtxProps.position.t - childProps.size.height;
      }
    } else if (myLayout.type == LayoutType::Column) {
      if (myLayout.major == Align::Start || myLayout.major == Align::Center) {
        if (allowMajorLeadingUpdates) {
          ctxProps.position.t += childProps.size.height + myLayout.gap;
        }
      } else if (myLayout.major == Align::End) {
        ctxProps.position.t -= childProps.size.height + myLayout.gap;
      }
      if (myLayout.minor == Align::Center) {
        ctxProps.position.l =
            initialCtxProps.position.l +
            (ctxProps.preferredSize.width / 2 - childProps.size.width / 2);
      } else if (myLayout.minor == Align::End) {
        ctxProps.position.l =
            initialCtxProps.position.l - childProps.size.width;
      }
    }
    if (ctxProps.position.l > ctxProps.preferredSize.width ||
        ctxProps.position.t > ctxProps.preferredSize.height) {
      overflow = true;
    }
    child->updateLayout(childCtx);
  }
}