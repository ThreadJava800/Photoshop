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
    if (regSet) delete regSet;

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

MPoint Widget::getSize() {
    return size;
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

bool Widget::getVisible() {
    return visible;
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

void Widget::setVisible(bool _visible) {
    visible = _visible;
}

bool Widget::onKeyboardPress(plugin::KeyboardContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists() && widget->visible) {
            wasClick = widget->onKeyboardPress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool Widget::onKeyboardRelease(plugin::KeyboardContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists()) widget->onKeyboardRelease(context);
    }

    return true;
}

bool Widget::onClock(uint64_t delta) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists()) widget->onClock(delta);
    }

    return true;
}

bool Widget::onMousePress(plugin::MouseContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists() && widget->visible && widget->isInside(MPoint(context.position))) {
            wasClick = widget->onMousePress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool Widget::onMouseRelease(plugin::MouseContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists()) widget->onMouseRelease(context);
    }

    return true;
}

bool Widget::onMouseMove(plugin::MouseContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget && widget->getExists()) widget->onMouseMove(context);
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

bool Widget::isInside(MPoint point) {
    return MathRectangle(position, size).isPointInside(point);
}

RegionSet* Widget::getDefaultRegSet() {
    createEmptyRegionSet();
    return regSet;
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
    Widget* test = this;
    while (test->parent != 0) test = test->parent;

    test->fillRegionSetsRoot();
}

void Widget::fillRegionSetsRoot() {
    if (!regSet || !regSet->getRectangles()) return;

    regSet = getDefaultRegSet();

    // if (!parent) {
    //     std::cout << size.x << ' ' << size.y << '\n';
    // }

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
                regSet->subtract(cur->getDefaultRegSet());
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
            regSet->subtract(cur->getDefaultRegSet());
        }
    }
}

void Widget::prioritizeWindow() {
    if (parent && parent->getWindows()) {
        parent->getWindows()->swapWithEnd(this);
        parent->fillRegionSets();
    }
}