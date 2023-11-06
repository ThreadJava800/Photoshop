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

bool Menu::onMousePressed(MPoint pos, MMouse btn) {
    if (isInsideBar(pos) && btn == LEFT) {
        isClicked = true;
        prevPos = pos;
    }

    bool retValue = isClicked;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];
        if (widget->getVisible()) {
            retValue |= widget->onMousePressed(pos, btn);
        }
    }

    return retValue;
}

bool Menu::onMouseReleased(MPoint pos, MMouse btn) {
    isClicked = false;
    return isInsideBar(pos);
}

bool Menu::onMouseMove(MPoint pos, MMouse btn) {
    if (isClicked) {
        if (window && onMove) {
            onMove(window, pos, prevPos);
            prevPos = pos;
        }
    }
    return true;
}

bool Menu::isInside(MPoint point) {
    size_t childrenCnt = subWindows->getSize();

    for (size_t i = 0; i < childrenCnt; i++) {
        Widget* child = (*subWindows)[i];
        if (child && child->getVisible()) {
            if (child->isInside(point)) return true;
        }
    }

    return Widget::isInside(point);
}

void Menu::render(RenderTarget* renderTarget) {
    regSet = getDefaultRegSet();
    Widget::render(renderTarget);
}