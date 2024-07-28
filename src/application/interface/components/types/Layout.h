#ifndef _LAYOUT_TYPES_H_
#define _LAYOUT_TYPES_H_

struct Position {
  int t = 0, b = 0, l = 0, r = 0;

  Position operator+(const Position &a) {
    return {a.t + t, a.b + b, a.l + l, a.r + r};
  }
};

struct Size {
  int width = 0, height = 0;
};

enum class LayoutType { Row, Column, None };

struct LayoutProps {
  int index = 0;
  int gap = 0;
};

enum class Align { Left, Right, Center };

struct LayoutContext {
  LayoutType type = LayoutType::None;
  Align align = Align::Left;
  LayoutProps props = {};
  Position padding = {};
  Position position = {};
  Size size = {};
  Size preferredSize = {};
};

LayoutContext deriveLayout(LayoutContext &parent);

#endif // _LAYOUT_TYPES_H_