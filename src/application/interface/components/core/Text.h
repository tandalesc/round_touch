#ifndef _TEXT_COMPONENT_H_
#define _TEXT_COMPONENT_H_

#include "src/application/interface/components/types/Component.h"

struct TextProps {
  uint8_t size = 4;
  uint16_t color = WHITE;
};

class Text : public Component {
  TextProps props;
  const char *text = "";

public:
  Text(TextProps props, const char *text) : props(props), text(text) {
    this->calculateSize();
  };
  Text(const char *text) : Text({}, text) {};

  void calculateSize() override {
    layout.size.width = strlen(text) * 6 * props.size;
    layout.size.height = 12 * props.size;
  }

  void render(Application *app) override {
    auto gfx = app->device()->display().gfx;
    gfx->setCursor(layout.position.l, layout.position.t);
    gfx->setTextSize(props.size);
    gfx->setTextColor(props.color);
    gfx->print(text);
  }
};

#endif