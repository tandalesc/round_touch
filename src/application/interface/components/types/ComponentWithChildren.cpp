#include "application/interface/components/types/ComponentWithChildren.h"

ComponentWithChildren::~ComponentWithChildren() {
  for (auto child : children) {
    delete child;
  }
}

void ComponentWithChildren::attachApplication(Application *app) {
  for (auto &child : children) {
    child->attachApplication(app);
  }
  Component::attachApplication(app);
}

void ComponentWithChildren::createWidgets(lv_obj_t *parent) {
  // by default, create a plain container object
  lvObj = lv_obj_create(parent);
  // make it transparent with no border/padding by default
  lv_obj_remove_style_all(lvObj);
  lv_obj_set_size(lvObj, LV_PCT(100), LV_SIZE_CONTENT);
  // create children inside this container
  for (auto &child : children) {
    child->createWidgets(lvObj);
  }
}

void ComponentWithChildren::handleEvent(InputEvent &event) {
  for (auto &child : children) {
    child->handleEvent(event);
  }
}
