#ifndef _CORE_FLEX_LAYOUT_H_
#define _CORE_FLEX_LAYOUT_H_

#include "application/interface/components/types/ComponentWithChildren.h"

class FlexLayout : public ComponentWithChildren {
private:
  LayoutContext initial;

public:
  template <typename... T>
  FlexLayout(LayoutContext layout, T *...children)
      : ComponentWithChildren(children...), initial(layout) {
    this->layout = layout;
  };

  FlexLayout(LayoutContext layout, std::vector<RenderableComponent> kids)
      : ComponentWithChildren(std::move(kids)), initial(layout) {
    this->layout = layout;
  };

  void createWidgets(lv_obj_t *parent) override;
};

#endif // _CORE_FLEX_LAYOUT_H_
