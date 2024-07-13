#ifndef _CORE_FLEX_LAYOUT_H_
#define _CORE_FLEX_LAYOUT_H_

#include <initializer_list>
#include <math.h>
#include <vector>

#include "src/application/Application.h"
#include "src/application/interface/components/types/Component.h"
#include "src/application/interface/components/types/Layout.h"

class FlexLayout : public Component {
  std::vector<RenderableComponent> children;

public:
  LayoutContext initial;
  FlexLayout(LayoutContext layout,
             std::initializer_list<RenderableComponent> children)
      : initial(layout), children{children} {
    this->layout = layout;
  };

  template <typename... T>
  FlexLayout(LayoutContext layout, T*... children)
      : initial(layout), children{children...} {
    this->layout = layout;
  };

  ~FlexLayout() {
    for (auto *child : this->children) {
      delete child;
    }
  }

  void calculateSize() override {
    int width = 0, height = 0;
    for (auto *child : this->children) {
      child->calculateSize();
      if (this->layout.list.type == ROW) {
        width += child->layout.size.width;
        height = max(height, child->layout.size.height);
      } else {
        width = max(width, child->layout.size.width);
        height += child->layout.size.height;
      }
    }
    this->layout.size = {width, height};
  }

  virtual void updateLayout(LayoutContext &layout) override {
    // update position based on derived position from parent
    LayoutContext derived = deriveLayout(layout);
    this->layout.position.l = this->initial.position.l + derived.position.l;
    this->layout.position.r = this->initial.position.r + derived.position.r;
    this->layout.position.t = this->initial.position.t + derived.position.t;
    this->layout.position.b = this->initial.position.b + derived.position.b;
    this->layout.list.position = derived.list.position;
    // copy context for application to list elements
    LayoutContext listContext = this->layout;
    for (int i = 0; i < this->children.size(); i++) {
      // apply layout props to each child
      auto *child = this->children[i];
      LayoutContext childContext = listContext;
      childContext.list.position = i;

      child->updateLayout(childContext);
      // update internals to ensure proper placement
      // update list context position
      if (listContext.list.type == ROW) {
        listContext.position.l += child->layout.size.width;
      } else {
        listContext.position.t += child->layout.size.height;
      }
    }
  }

  void drawDebugBounds(Application *app) {
    app->device()->display().gfx->drawRect(
        this->layout.position.l + this->layout.padding.l,
        this->layout.position.t + this->layout.padding.t,
        this->layout.size.width, this->layout.size.height, RED);
  }

  virtual void render(Application *app) override {
    for (auto *child : this->children) {
      child->render(app);
    }
    // this->drawDebugBounds(app);
  }
};

#endif // _CORE_FLEX_LAYOUT_H_