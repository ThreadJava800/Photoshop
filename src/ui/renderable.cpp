#include "renderable.h"

Widget::Widget(MPoint _position, MPoint _size) :
    position(_position),
    size    (_size),
    exists  (true) {
        subWindows = new List<Widget*>();
    }

Widget::Widget(MPoint _position, MPoint _size, List<Widget*>* _subWindows) :
    position  (_position),
    size      (_size),
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

List<RegionSet*>* Widget::createRegions() {
    ON_ERROR(!subWindows, "Widget list was null!", nullptr);

    // recursion base
    size_t listSize = subWindows->getSize();
    if (listSize == 0) {
        // MathRectangle thisRect = MathRectangle(position, size);
        // RegionSet* thisRegSet  = new RegionSet();
        // thisRegSet->addRegion(thisRect);

        // List<RegionSet*>* rlist = new List<RegionSet*>();
        // rlist->pushBack(thisRegSet);

        return nullptr;
    }

    List<RegionSet*>* result = new List<RegionSet*>(listSize);

    MathRectangle upperW = MathRectangle((*subWindows)[listSize - 1]->position, (*subWindows)[listSize - 1]->size);
    RegionSet* upperRegSet = new RegionSet();
    upperRegSet->addRegion(upperW);
    (*result)[listSize - 1] = upperRegSet;

    for (long i = listSize -  2; i >= 0; i--) {
        MathRectangle thisW = MathRectangle((*subWindows)[i]->position, (*subWindows)[i]->size);
        RegionSet* thisWRegSet = new RegionSet();
        thisWRegSet->addRegion(thisW);
        (*result)[i] = thisWRegSet;

        for (long j = i + 1; j < listSize; j++) {
            (*result)[i]->subtract((*result)[j]);
        }
    }

    return result;
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