#include "src/application/interface/components/types/ComponentWithChildren.h"

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

void ComponentWithChildren::calculateSize() {
  for (auto &child : children) {
    child->calculateSize();
  }
}

void ComponentWithChildren::updateLayout(LayoutContext &layout) {
  for (auto &child : children) {
    child->updateLayout(layout);
  }
}

void ComponentWithChildren::handleEvent(InputEvent &event) {
  for (auto &child : children) {
    child->handleEvent(event);
  }
}

void ComponentWithChildren::render(Application *app) {
  for (auto &child : children) {
    child->render(app);
  }
}