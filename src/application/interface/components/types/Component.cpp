#include "application/interface/components/types/Component.h"

void Component::attachApplication(Application *app) { this->app = app; }

void Component::updateLayout(LayoutContext &layout) {
  // update position only.
  // rest of properties might be overridden for other purposes
  LayoutContext derived = deriveLayout(layout);
  this->layout.position = derived.position;
}

void Component::drawDebugBounds(Application *app) {
  app->device()->display().gfx->drawRect(
      layout.position.l,
      layout.position.t,
      layout.preferredSize.width + layout.padding.l + layout.padding.r,
      layout.preferredSize.height + layout.padding.t + layout.padding.b, RED);
}