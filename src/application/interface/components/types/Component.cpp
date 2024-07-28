#include "src/application/interface/components/types/Component.h"

void Component::attachApplication(Application *app) { this->app = app; }

void Component::updateLayout(LayoutContext &layout) {
  // update position only.
  // rest of properties might be overridden for other purposes
  LayoutContext derived = deriveLayout(layout);
  this->layout.props.position = derived.props.position;
}

void Component::drawDebugBounds(Application *app) {
  Size sizeIncludingPadding = layout.props.preferredSize + layout.padding;
  app->device()->display().gfx->drawRect(
      layout.props.position.l, layout.props.position.t,
      sizeIncludingPadding.width, sizeIncludingPadding.height, RED);
}