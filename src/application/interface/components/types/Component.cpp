#include "application/interface/components/types/Component.h"

Component::~Component() {
  // LVGL objects are cleaned up when the screen is deleted
  lvObj = nullptr;
}

void Component::attachApplication(Application *app) { this->app = app; }
