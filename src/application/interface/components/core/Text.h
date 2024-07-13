#ifndef _TEXT_COMPONENT_H_
#define _TEXT_COMPONENT_H_

#include "src/application/Application.h"
#include "src/application/interface/components/types/Component.h"
#include "src/application/interface/components/types/Layout.h"

struct TextProps {
  const char *text = "";
  uint16_t color = WHITE;
  uint8_t size = 4;
};

class Text : public Component {
  TextProps props;

public:
  Text(TextProps props) : props(props) { this->calculateSize(); };

  void calculateSize() override {
    this->layout.size.width = strlen(props.text) * 6 * props.size;
    this->layout.size.height = 12 * props.size;
  }

  void render(Application *app) override {
    auto gfx = app->device()->display().gfx;
    gfx->setCursor(this->layout.position.l, this->layout.position.t);
    gfx->setTextSize(this->props.size);
    gfx->setTextColor(this->props.color);
    gfx->print(this->props.text);
  }
};

#endif