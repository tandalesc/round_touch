#ifndef _TEXT_COMPONENT_H_
#define _TEXT_COMPONENT_H_

#include "application/interface/components/types/Component.h"

struct TextProps {
  uint8_t size = 4;
  uint16_t color = WHITE;
};

class Text : public Component {
  TextProps props;
  const char *text = "";

public:
  Text(TextProps props, const char *text) : props(props), text(text) {
    this->initializeSize();
  };
  Text(const char *text) : Text({}, text) {};

  void initializeSize() {
    layout.size.width = strlen(text) * 6 * props.size;
    layout.size.height = 8 * props.size;
    layout.preferredSize = layout.size;
  }

  void calculateSize(LayoutContext &layout) override { initializeSize(); }

  void render(Application *app) override {
    auto gfx = app->device()->display().gfx();
    gfx->setCursor(layout.position.l, layout.position.t);
    gfx->setTextSize(props.size);
    gfx->setTextColor(props.color);
    gfx->print(text);
  }
};

#endif