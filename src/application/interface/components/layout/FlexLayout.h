#ifndef _CORE_FLEX_LAYOUT_H_
#define _CORE_FLEX_LAYOUT_H_

#define DRAW_DEBUG true

#include "src/application/interface/components/types/ComponentWithChildren.h"

class FlexLayout : public ComponentWithChildren {
private:
  LayoutContext initial;

public:
  template <typename... T>
  FlexLayout(LayoutContext layout, T *...children)
      : ComponentWithChildren(children...), initial(layout) {
    this->layout = layout;
  };

  virtual void calculateSize(LayoutContext &layout) override;
  virtual void updateLayout(LayoutContext &layout) override;

#if DRAW_DEBUG
  void render(Application *app) override {
    ComponentWithChildren::render(app);
    drawDebugBounds(app);
  }
#endif
};

#endif // _CORE_FLEX_LAYOUT_H_