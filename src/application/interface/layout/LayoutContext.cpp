#include "src/application/interface/layout/LayoutContext.h"

Position Position::operator+(const Position &a) {
  return {a.t + t, a.b + b, a.l + l, a.r + r};
}

Size Size::operator+(const Size &a) {
  return {width + a.width, height + a.height};
}

Size Size::operator+(const Position &a) {
  return {width + a.l + a.r, height + a.t + a.b};
}

Size Size::operator-(const Position &a) {
  return {width - a.l - a.r, height - a.t - a.b};
}

Size Size::operator*(const unsigned int &a) {
  return {width*a, height*a};
}

Size Size::operator/(const unsigned int &a) {
  return {width/a, height/a};
}

Size operator*(const int &a, const Size &b) {
  return a*b;
}

LayoutContext deriveLayout(LayoutContext &parent) {
  LayoutContext clone = parent;
  clone.props.position = parent.props.position + parent.padding;
  clone.props.preferredSize = parent.props.preferredSize - parent.padding;
  clone.props.size = parent.props.size - parent.padding;
  return clone;
}