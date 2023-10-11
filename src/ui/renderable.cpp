#include "renderable.h"

Widget::Widget(MPoint _position, MPoint _size, Widget* _parent) :
    position(_position),
    size    (_size),
    parent  (_parent),
    exists  (true) {
        subWindows = new List<Widget*>();
        createEmptyRegionSet();
    }

Widget::Widget(MPoint _position, MPoint _size, Widget* _parent, List<Widget*>* _subWindows) :
    position  (_position),
    size      (_size),
    parent    (_parent),
    subWindows(_subWindows),
    exists    (true)  {
        createEmptyRegionSet();
    }

void Widget::createEmptyRegionSet() {
    regSet = new RegionSet();
    regSet->addRegion(MathRectangle(position, size));
}

Widget::~Widget() {
    if (!subWindows) return;

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) delete widget;
    }

    delete subWindows;
    subWindows = nullptr;

    delete regSet;
    regSet = nullptr;
}

MPoint Widget::getPosition() {
    return position;
}

List<Widget*>* Widget::getWindows() {
    return subWindows;
}

void Widget::setExists(bool val) {
    exists = val;
}

bool Widget::getExists() {
    return exists;
}

RegionSet* Widget::getRegSet() {
    return regSet;
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

void Widget::registerObject(Widget* widget) {
    ON_ERROR(!widget, "Widget ptr was null!",)

    RegionSet* curRegSet = new RegionSet();
    curRegSet->addRegion(MathRectangle(widget->position, widget->size));
    subWindows->pushBack(widget);
    updateRegions(curRegSet);
    delete curRegSet;
}

void Widget::updateRegions(RegionSet* subSet) {
    regSet->subtract(subSet);
}