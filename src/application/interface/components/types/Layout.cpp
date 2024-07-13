#include "src/application/interface/components/types/Layout.h"

LayoutContext deriveLayout(LayoutContext &parent) {
  LayoutContext clone = parent;
  clone.position.t = parent.position.t + parent.padding.t;
  clone.position.b = parent.position.b + parent.padding.b;
  clone.position.l = parent.position.l + parent.padding.l;
  clone.position.r = parent.position.r + parent.padding.r;
  clone.size.width = parent.size.width - parent.padding.l - parent.padding.r;
  clone.size.height = parent.size.height - parent.padding.t - parent.padding.b;
  return clone;
}