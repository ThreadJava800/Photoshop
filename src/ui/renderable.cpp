#include "renderable.h"

Widget::Widget(MPoint _position) :
    position(_position),
    exists  (true) {
        subWindows = new List<Widget*>();
    }

Widget::Widget(MPoint _position, List<Widget*>* _subWindows) :
    position  (_position),
    subWindows(_subWindows),
    exists    (true)  {}

Widget::~Widget() {
    if (!subWindows) return;

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) delete widget;
    }

    delete subWindows;
    subWindows = nullptr;
}

MPoint Widget::getPosition() {
    return position;
}

void Widget::setExists(bool val) {
    exists = val;
}

bool Widget::getExists() {
    return exists;
}

bool Widget::onMousePressed(MPoint pos, MMouse btn) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists()) {
            wasClick = widget->onMousePressed(pos, btn);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool Widget::onMouseReleased(MPoint pos, MMouse btn) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists()) widget->onMouseReleased(pos, btn);
    }

    return true;
}

bool Widget::onMouseMove(MPoint pos, MMouse btn) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists()) widget->onMouseMove(pos, btn);
    }

    return true;
}

void Widget::move(MPoint shift) {
    ON_ERROR(!subWindows, "List pointer was null!",);

    position += shift;
    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists()) {
            widget->move(shift);
        }
    }
}