#include "src/application/interface/layout/LayoutManager.h"

void LayoutManager::attach(RenderableComponent *root) {
    this->root = root;
}

void LayoutManager::detatch() {
    this->root = nullptr;
}

void updateSizes() {
    LayoutContext *layout = &root;
    while(layout != nullptr) {
        LayoutContext derived = deriveLayout(*layout);
        
    }
}

void updatePositions() {

}

void LayoutManager::reflow() {
    updateSizes()
    updatePositions();
}