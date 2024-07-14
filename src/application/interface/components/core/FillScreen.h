#ifndef _FILL_SCREEN_COMPONENT_H_
#define _FILL_SCREEN_COMPONENT_H_

#include "src/application/interface/components/types/ComponentWithChildren.h"

struct FillScreenProps {
  uint16_t color = BLACK;
};

class FillScreen : public ComponentWithChildren {
private:
  FillScreenProps props;

public:
  template <typename... T>
  FillScreen(FillScreenProps props, T *...children)
      : ComponentWithChildren(children...), props(props){};

  // applies gfx->fillScreen before rendering children
  void render(Application *app) override {
    auto gfx = app->device()->display().gfx;
    gfx->fillScreen(this->props.color);
    ComponentWithChildren::render(app);
  }
};

#endif // _FILL_SCREEN_COMPONENT_H_