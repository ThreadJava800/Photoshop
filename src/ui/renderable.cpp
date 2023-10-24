#include "renderable.h"

Widget::Widget(MPoint _position, MPoint _size, Widget* _parent, uint8_t _priority) :
    EventProcessable(_priority),
    position(_position),
    size    (_size),
    parent  (_parent),
    visible (true),
    exists  (true) {
        debColor = MColor(DEB_COLS[rand() % DEB_COLS_CNT]);
        subWindows = new List<Widget*>();
        createEmptyRegionSet();
    }

Widget::Widget(MPoint _position, MPoint _size, Widget* _parent, List<Widget*>* _subWindows, uint8_t _priority) :
    EventProcessable(_priority),
    position  (_position),
    size      (_size),
    parent    (_parent),
    subWindows(_subWindows),
    visible   (true),
    exists    (true)  {
        debColor = MColor(DEB_COLS[rand() % DEB_COLS_CNT]);
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

Widget* Widget::getParent  () {
    return parent;
}

void Widget::setParent(Widget* _parent) {
    parent = _parent;
}

bool Widget::onMousePressed(MPoint pos, MMouse btn) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists() && widget->visible) {
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

    if (regSet) {
        size_t regionCnt = regSet->getSize();
        for (size_t i = regionCnt; i < regionCnt; i++) {
            MathRectangle moved = MathRectangle((*regSet)[i].getPosition() + shift, (*regSet)[i].getSize());
            (*regSet)[i] = moved;
        }
    }

    size_t listSize = subWindows->getSize();
    position += shift;
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists()) {
            widget->move(shift);
        }
    }
}

void Widget::registerObject(Widget* widget) {
    ON_ERROR(!widget, "Widget ptr was null!",)

    widget->setParent(this);
    subWindows->pushBack(widget);

    fillRegionSets();  
}

void Widget::render(RenderTarget* renderTarget) {
    unregisterObject();

    if (visible) {
        size_t listSize = subWindows->getSize();
        for (size_t i = 0; i < listSize; i++) {
            Widget* widget = (*subWindows)[i];
            if (widget) {
                widget->render(renderTarget);
            }
        }

        // regSet->visualize(renderTarget, debColor);
    }
}

void Widget::unregisterObject() {
    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];
        if (widget && !widget->getExists()) {
            Widget* delParent = widget->getParent();

            delete widget;
            subWindows->remove(i);
            listSize--;

            fillRegionSets();
        }
    }
}

void Widget::clearRegionSets() {
    size_t listSize = subWindows->getSize();

    regSet->getRectangles()->clear();

    for (size_t i = 0; i < listSize; i++) {
        (*subWindows)[i]->clearRegionSets();
    }
}

void Widget::fillRegionSets() {
    int cnt = 0;
    Widget* test = this;
    while (test->parent != 0) test = test->parent;

    test->fillRegionSetsRoot();
}

void Widget::fillRegionSetsRoot() {
    if (!regSet || !regSet->getRectangles()) return;

    regSet->getRectangles()->clear();

    MathRectangle thisRect = MathRectangle(position, size);
    regSet->addRegion(thisRect);

    if (parent) {
        RegionSet* oldRegSet = regSet;
        regSet = regSet->cross(parent->regSet);
        delete oldRegSet;

        List<Widget*>* parentWins = parent->subWindows;
        size_t         parentCCnt = parentWins->getSize();
        size_t         valInd     = 0;
        for (size_t i = 0; i < parentCCnt; i++) {
            if (this == (*parentWins)[i]) {
                valInd = i + 1;
                break;
            }
        }

        for (size_t i = valInd; i < parentCCnt; i++) {
            Widget* cur = (*parentWins)[i];

            if (cur->visible) {
                MathRectangle curRect = MathRectangle(cur->position, cur->size);

                RegionSet curSet = RegionSet();
                curSet.addRegion(curRect);

                regSet->subtract(&curSet);
            }
        }
    }

    size_t childCnt = subWindows->getSize();

    for (size_t i = 0; i < childCnt; i++) {
        Widget *subWin = (*subWindows)[i];
        if (subWin->visible) subWin->fillRegionSetsRoot();
    }

    // minus children
    for (size_t i = 0; i < childCnt; i++) {
        Widget* cur = (*subWindows)[i];

        if (cur->visible) {
            MathRectangle curRect = MathRectangle(cur->position, cur->size);

            RegionSet curSet = RegionSet();
            curSet.addRegion(curRect);

            regSet->subtract(&curSet);
        }
    }
}