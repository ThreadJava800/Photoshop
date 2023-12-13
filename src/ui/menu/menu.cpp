#include "menu.h"

Menu::Menu(MPoint _position, MPoint _size, Widget* _parent, Window* _window, OnMoveFunc _onMove) :
    Widget   (_position, _size, _parent),
    isClicked(false),
    window   (_window),
    onMove   (_onMove),
    prevPos  (MPoint())     {}

Menu::~Menu() {}

RegionSet* Menu::getDefaultRegSet() {
    size_t childrenCnt = subWindows->getSize();

    // reset region set
    createEmptyRegionSet();

    for (size_t i = 0; i < childrenCnt; i++) {
        Widget* child = (*subWindows)[i];

        if (child->getVisible()) {
            // MathRectangle r = (*child->getRegSet()->getRectangles())[0];
            // std::cout << r.getSize().x << ' ' << r.getSize().y << '\n';
            regSet->merge(child->getRegSet());
        }
    } 

    return regSet;
}

bool Menu::isInsideBar(MPoint checkPoint) {
    return checkPoint.x - position.x >= 0      &&
           checkPoint.x - position.x <= size.x &&
           checkPoint.y - position.y >= 0      &&
           checkPoint.y - position.y <= size.y;
}

bool Menu::onMousePress(plugin::MouseContext context) {
    if (isInsideBar(MPoint(context.position)) && context.button == plugin::MouseButton::Left) {
        isClicked = true;
        prevPos = MPoint(context.position);
    }

    bool retValue = isClicked;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];
        if (widget->getVisible()) {
            retValue |= widget->onMousePress(context);
        }
    }

    return retValue;
}

bool Menu::onMouseRelease(plugin::MouseContext context) {
    isClicked = false;
    return isInsideBar(MPoint(context.position));
}

bool Menu::onMouseMove(plugin::MouseContext context) {
    if (isClicked) {
        if (window && onMove) {
            onMove(window, MPoint(context.position), prevPos);
            prevPos = MPoint(context.position);
        }
    }
    return true;
}

bool Menu::isInside(MPoint point) {
    size_t childrenCnt = subWindows->getSize();

    for (size_t i = 0; i < childrenCnt; i++) {
        Widget* child = (*subWindows)[i];
        if (child->getVisible()) {
            if (child->isInside(point)) return true;
        }
    }

    return Widget::isInside(point);
}

void Menu::render(RenderTarget* renderTarget) {
    // regSet = getDefaultRegSet();
    Widget::render(renderTarget);
}