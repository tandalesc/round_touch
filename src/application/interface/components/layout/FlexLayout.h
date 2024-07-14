#ifndef _CORE_FLEX_LAYOUT_H_
#define _CORE_FLEX_LAYOUT_H_

#include <math.h>
#include <vector>

#include "src/application/Application.h"
#include "src/application/interface/components/types/Component.h"
#include "src/application/interface/components/types/Layout.h"

class FlexLayout : public Component {
  std::vector<RenderableComponent> children;

public:
  LayoutContext initial;

  template <typename... T>
  FlexLayout(LayoutContext layout, T *...children)
      : initial(layout), children{children...} {
    this->layout = layout;
  };

  ~FlexLayout();

  virtual void calculateSize() override;
  virtual void render(Application *app) override;
  virtual void updateLayout(LayoutContext &layout) override;

  void drawDebugBounds(Application *app);
};

#endif // _CORE_FLEX_LAYOUT_H_