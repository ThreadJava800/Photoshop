#include "renderable.h"

Widget::Widget(MPoint _position) :
    position(_position) {
        subWindows = new List<Widget*>();
    }

Widget::Widget(MPoint _position, List<Widget*>* _subWindows) :
    position  (_position),
    subWindows(_subWindows)  {}

Widget::~Widget() {
    delete subWindows;

    subWindows = nullptr;
}

MPoint Widget::getPosition() {
    return position;
}

bool Widget::onMousePressed(MPoint pos, MMouse btn) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) widget->onMousePressed(pos, btn);
    }

    return true;
}