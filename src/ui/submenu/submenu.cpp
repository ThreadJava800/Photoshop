#include "submenu.h"

SubMenu::SubMenu(MPoint _position, MPoint _size, Widget* _parent) :
    Menu(_position, _size, _parent)    {
        visible = false;
    }

SubMenu::~SubMenu() {}

void SubMenu::changeActivity() {
    visible = !visible;
    // fillRegionSets();

    if (visible) prioritizeWindow();
    if (parent)  parent->prioritizeWindow();
}

void SubMenu::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Drawable area was null!",);

    if (!visible) return;

    renderTarget->drawRect (position, size, DEFAULT_BACK_COL, MColor::TRANSPARENT, regSet);

    Widget::render(renderTarget);

    renderTarget->drawFrame(position, size, MColor::GRAY, regSet);
}

RegionSet* SubMenu::getDefaultRegSet() {
    size_t childrenCnt = subWindows->getSize();

    // reset region set
    createEmptyRegionSet();

    for (size_t i = 0; i < childrenCnt; i++) {
        Widget* child = (*subWindows)[i];

        if (child->getVisible()) {
            regSet->merge(child->getRegSet());
        }
    } 

    return regSet;
}