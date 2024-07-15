#include "src/application/interface/components/types/Component.h"

void Component::attachApplication(Application *app) {
  this->app = app;
}

void Component::updateLayout(LayoutContext &layout) {
  // update position only.
  // rest of properties might be overridden for other purposes
  LayoutContext derived = deriveLayout(layout);
  this->layout.position = derived.position;
}

void Component::drawDebugBounds(Application *app) {
  app->device()->display().gfx->drawRect(
      this->layout.position.l + this->layout.padding.l,
      this->layout.position.t + this->layout.padding.t, this->layout.size.width,
      this->layout.size.height, RED);
}