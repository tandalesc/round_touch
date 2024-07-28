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
    this->initializeSize();
  };
  Text(const char *text) : Text({}, text) {};

  void initializeSize() {
    layout.props.size.width = strlen(text) * 6 * props.size;
    layout.props.size.height = 8 * props.size;
    layout.props.preferredSize = layout.props.size;
  }

  void calculateSize(LayoutContext &layout) override { initializeSize(); }

  void render(Application *app) override {
    auto gfx = app->device()->display().gfx;
    Location cursor(layout.props.position);
    gfx->setCursor(cursor.x, cursor.y);
    gfx->setTextSize(props.size);
    gfx->setTextColor(props.color);
    gfx->print(text);
  }
};

#endif