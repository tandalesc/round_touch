#ifndef _FILL_SCREEN_COMPONENT_H_
#define _FILL_SCREEN_COMPONENT_H_

#include "src/application/Application.h"
#include "src/application/interface/components/types/Component.h"
#include "src/application/interface/components/types/Layout.h"

struct FillScreenProps {
  uint16_t color = BLACK;
};

class FillScreen : public Component {
private:
  FillScreenProps props;
  Component *child;

public:
  FillScreen(FillScreenProps props, Component *child)
      : props(props), child(child) {};
  ~FillScreen() override { delete child; }

  void calculateSize() override {
    this->child->calculateSize();
  }

  void updateLayout(LayoutContext &layout) override {
    this->child->updateLayout(layout);
  }

  void render(Application *app) override {
    auto gfx = app->device()->display().gfx;
    gfx->fillScreen(this->props.color);
    this->child->render(app);
  }
};

#endif // _FILL_SCREEN_COMPONENT_H_