#include "menu.h"

Menu::Menu(MPoint _position, MPoint _size, Widget* _parent, Window* _window, OnMoveFunc _onMove, PriorFunc _priorFun) :
    Widget   (_position, _size, _parent),
    isClicked(false),
    window   (_window),
    onMove   (_onMove),
    prior    (_priorFun),
    prevPos  (MPoint())     {}

Menu::~Menu() {}

bool Menu::isInside(MPoint checkPoint) {
    return checkPoint.x - position.x >= 0      &&
           checkPoint.x - position.x <= size.x &&
           checkPoint.y - position.y >= 0      &&
           checkPoint.y - position.y <= size.y;
}

bool Menu::onMousePressed(MPoint pos, MMouse btn) {
    if (isInside(pos) && btn == LEFT && !Widget::onMousePressed(pos, btn)) {
        isClicked = true;
        prevPos = pos;

        if (prior) prior(window);
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