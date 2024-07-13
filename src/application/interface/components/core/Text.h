#ifndef _TEXT_COMPONENT_H_
#define _TEXT_COMPONENT_H_

#include "src/application/Application.h"
#include "src/application/interface/components/core/Component.h"

struct TextProps {
  const char *text = "";
  uint16_t color = WHITE;
  uint8_t size = 4;
};

class Text : public Component {
  TextProps props;

public:
  Text(TextProps props) : props(props) {};
  void render(Application *app) override {
    auto gfx = app->device()->display().gfx;
    gfx->setTextSize(this->props.size);
    gfx->setTextColor(this->props.color);
    gfx->setCursor(10, 80);
    gfx->println(this->props.text);
  }
};

#endif