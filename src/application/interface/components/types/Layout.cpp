#include "src/application/interface/components/types/Layout.h"

LayoutContext deriveLayout(LayoutContext &parent) {
  LayoutContext clone = parent;
  clone.position = parent.position + parent.padding;
  clone.size.width = parent.size.width - parent.padding.l - parent.padding.r;
  clone.size.height = parent.size.height - parent.padding.t - parent.padding.b;
  clone.preferredSize.width = parent.preferredSize.width - parent.padding.l - parent.padding.r;
  clone.preferredSize.height = parent.preferredSize.height - parent.padding.t - parent.padding.b;
  return clone;
}