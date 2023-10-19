#include "submenu.h"

SubMenu::SubMenu(MPoint _position, MPoint _size, Widget* _parent) :
    Widget(_position, _size, _parent)    {
        visible = false;
    }

SubMenu::~SubMenu() {}

void SubMenu::changeActivity() {
    visible = !visible;
}

void SubMenu::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Drawable area was null!",);

    if (!visible) return;

    renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL), MColor(GRAY), regSet);

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) {
            widget->render(renderTarget);
        }
    }

    Widget::render(renderTarget);
}