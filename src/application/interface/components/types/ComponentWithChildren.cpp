#include "src/application/interface/components/types/ComponentWithChildren.h"

ComponentWithChildren::~ComponentWithChildren() {
  for (auto child : children) {
    delete child;
  }
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

void ComponentWithChildren::setupEventListeners(Application *app) {
  for (auto &child : children) {
    child->setupEventListeners(app);
  }
}

void ComponentWithChildren::render(Application *app) {
  for (auto &child : children) {
    child->render(app);
  }
}