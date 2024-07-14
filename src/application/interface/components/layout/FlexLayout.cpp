#include "src/application/interface/components/layout/FlexLayout.h"

FlexLayout::~FlexLayout() {
  for (auto *child : this->children) {
    delete child;
  }
}

void FlexLayout::calculateSize() {
  int width = 0, height = 0;
  for (auto *child : this->children) {
    child->calculateSize();
    if (this->layout.list.type == ROW) {
      width += child->layout.size.width;
      height = max(height, child->layout.size.height);
    } else {
      width = max(width, child->layout.size.width);
      height += child->layout.size.height;
    }
  }
  this->layout.size = {width, height};
}

void FlexLayout::updateLayout(LayoutContext &layout) {
  // update position based on derived position from parent
  LayoutContext derived = deriveLayout(layout);
  this->layout.position = this->initial.position + derived.position;
  this->layout.list.position = derived.list.position;
  // copy context for application to list elements
  LayoutContext listContext = this->layout;
  for (int i = 0; i < this->children.size(); i++) {
    // apply layout props to each child
    auto *child = this->children[i];
    LayoutContext childContext = listContext;
    childContext.list.position = i;
    child->updateLayout(childContext);
    // update internals to ensure proper placement
    // update list context position
    if (listContext.list.type == ROW) {
      listContext.position.l += child->layout.size.width;
    } else {
      listContext.position.t += child->layout.size.height;
    }
  }
}

void FlexLayout::drawDebugBounds(Application *app) {
  app->device()->display().gfx->drawRect(
      this->layout.position.l + this->layout.padding.l,
      this->layout.position.t + this->layout.padding.t, this->layout.size.width,
      this->layout.size.height, RED);
}

void FlexLayout::render(Application *app) {
  for (auto *child : this->children) {
    child->render(app);
  }
  // this->drawDebugBounds(app);
}