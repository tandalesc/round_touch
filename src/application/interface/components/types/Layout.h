#ifndef _LAYOUT_TYPES_H_
#define _LAYOUT_TYPES_H_

struct Padding {
  int t = 0, b = 0, l = 0, r = 0;
};

struct Position {
  int t = 0, b = 0, l = 0, r = 0;
};

struct Size {
  int width = 0, height = 0;
};

enum ListType { ROW, COLUMN };

struct ListProps {
  int position = 0;
  ListType type = COLUMN;
};

struct LayoutContext {
  ListProps list = {};
  Padding padding = {};
  Position position = {};
  Size size = {};
};

LayoutContext deriveLayout(LayoutContext &parent);

#endif // _LAYOUT_TYPES_H_