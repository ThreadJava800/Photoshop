#include "menu.h"

Menu::Menu(MPoint _position, MPoint _size, Window* _window, OnMoveFunc _onMove) :
    Widget   (_position),
    isClicked(false),
    size     (_size),
    window   (_window),
    onMove   (_onMove),
    prevPos  (MPoint())     {}

Menu::~Menu() {}

void Menu::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);
    ON_ERROR(!subWindows, "Buttons pointer was null!",);

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* btn = (*subWindows)[i];
        if (btn) btn->render(renderTarget);
    }
}

bool Menu::isInside(MPoint checkPoint) {
    return checkPoint.x - position.x >= 0      &&
           checkPoint.x - position.x <= size.x &&
           checkPoint.y - position.y >= 0      &&
           checkPoint.y - position.y <= size.y;
}

void Menu::registerObject(Widget* widget) {
    subWindows->pushBack(widget);
}

bool Menu::onMousePressed(MPoint pos, MMouse btn) {
    if (isInside(pos) && btn == LEFT && !Widget::onMousePressed(pos, btn)) {
        isClicked = true;
        prevPos = pos;
    }

    return isClicked;
}

bool Menu::onMouseReleased(MPoint pos, MMouse btn) {
    isClicked = false;
    return isInside(pos);
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