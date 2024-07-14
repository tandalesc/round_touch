#ifndef _CORE_FLEX_LAYOUT_H_
#define _CORE_FLEX_LAYOUT_H_

#include <math.h>
#include <vector>

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

  virtual void calculateSize() override;
  virtual void updateLayout(LayoutContext &layout) override;
};

#endif // _CORE_FLEX_LAYOUT_H_