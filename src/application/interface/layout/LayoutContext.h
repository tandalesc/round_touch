#pragma once

struct Position {
  int t = 0, b = 0, l = 0, r = 0;
  Position operator+(const Position &a);
};

struct Location {
  int x = 0, y = 0;
  Location(Position &p) : x(p.l), y(p.t) {};
};

struct Size {
  int width = 0, height = 0;
  Size operator+(const Size &a);
  Size operator+(const Position &a);
  Size operator-(const Position &a);
};

enum class LayoutType { Row, Column, None };
enum class Align { Start, End, Center };

struct ContainerProps {
  Position position = {};
  Size size = {};
  Size preferredSize = {};
  int index = 0;
};

struct LayoutContext {
  LayoutType type = LayoutType::None;
  Align major = Align::Start;
  Align minor = Align::Start;
  int gap = 0;
  Position padding = {};
  ContainerProps props = {};
};

LayoutContext deriveLayout(LayoutContext &parent);