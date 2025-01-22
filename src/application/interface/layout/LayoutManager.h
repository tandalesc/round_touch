#pragma once

#include "src/application/interface/components/types/Component.h"
#include "src/application/interface/layout/LayoutContext.h"

namespace RoundLCD {
LayoutContext rootContext = {
    .props = {.position = {.t = 40, .b = 40, .l = 12, .r = 12},
              .preferredSize = {.width = SCREEN_MAX_WIDTH - (12 * 2),
                                .height = SCREEN_MAX_HEIGHT - (40 * 2)}}};
};

class LayoutManager {
private:
  RenderableComponent *root = nullptr;
  LayoutContext rootContext;

  void updateSizes();
  void updatePositions();

public:
    LayoutManager(LayoutContext rootContext): rootContext(rootContext) {};

    void attach(RenderableComponent *root);
    void detatch();
    void reflow();
};