#include "renderable.h"

Widget::Widget(MPoint _position, MPoint _size, Widget* _parent, uint8_t _priority) :
    position(_position),
    size    (_size),
    parent  (_parent),
    visible (true),
    exists  (true),
    priority(_priority) {
        debColor = MColor(DEB_COLS[rand() % DEB_COLS_CNT]);
        subWindows = new List<Widget*>();
        createEmptyRegionSet();
    }

Widget::Widget(MPoint _position, MPoint _size, Widget* _parent, List<Widget*>* _subWindows, uint8_t _priority) :
    position  (_position),
    size      (_size),
    parent    (_parent),
    subWindows(_subWindows),
    visible   (true),
    exists    (true),
    priority  (_priority)  {
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
        plugin::WidgetI* widget = (*subWindows)[i];
        delete widget;
    }

    delete subWindows;
    subWindows = nullptr;

    delete regSet;
    regSet = nullptr;
}

void Widget::registerSubWidget(plugin::WidgetI* object) {
    ON_ERROR(!object, "Widget ptr was null!",)

    object->setParent(this);
    subWindows->pushBack((Widget*)object);
}

void Widget::unregisterSubWidget(plugin::WidgetI* object) {
    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        plugin::WidgetI* widget = (*subWindows)[i];
        if (widget  == object)
            subWindows->remove(i);
    }
}

plugin::Vec2 Widget::getSize() const {
    return size.toVec2();
}

void Widget::setSize(plugin::Vec2 _size) {
    size = MPoint(_size);
}

plugin::Vec2 Widget::getPos() const {
    return position.toVec2();
}

void Widget::setPos(plugin::Vec2 _pos) {
    position = MPoint(_pos);
}

void Widget::setParent(WidgetI *root) {
    parent = (Widget*) root;
}

plugin::WidgetI* Widget::getParent() const {
    return parent;
}

void Widget::move(plugin::Vec2 shift) {
    ON_ERROR(!subWindows, "List pointer was null!",);

    if (regSet) {
        size_t regionCnt = regSet->getSize();
        for (size_t i = regionCnt; i < regionCnt; i++) {
            MathRectangle moved = MathRectangle(MPoint((*regSet)[i].getPosition()) + MPoint(shift), (*regSet)[i].getSize());
            (*regSet)[i] = moved;
        }
    }

    size_t listSize = subWindows->getSize();
    position += MPoint(shift);
    for (size_t i = 0; i < listSize; i++) {
        plugin::WidgetI* widget = (*subWindows)[i];

        if (widget->getAvailable()) {
            widget->move(shift);
        }
    }
}

bool Widget::getAvailable() const {
    return exists;
}

void Widget::setAvailable(bool _available) {
    exists = _available;
}

void Widget::recalcRegion() {
    fillRegionSetsRoot();
}

uint8_t Widget::getPriority() const {
    return priority;
}

List<Widget*>* Widget::getWindows() {
    return subWindows;
}

bool Widget::getVisible() {
    return visible;
}

RegionSet* Widget::getRegSet() {
    return regSet;
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

        if (widget->getAvailable()) {
            wasClick = widget->onKeyboardPress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool Widget::onKeyboardRelease(plugin::KeyboardContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget->getAvailable()) wasClick = widget->onKeyboardRelease(context);
        if (wasClick) return true;
    }

    return false;
}

bool Widget::onClock(uint64_t delta) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget->getAvailable()) wasClick = widget->onClock(delta);
        if (wasClick) return true;
    }

    return false;
}

bool Widget::onMousePress(plugin::MouseContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget->getAvailable()) {
            wasClick = widget->onMousePress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool Widget::onMouseRelease(plugin::MouseContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget->getAvailable()) wasClick = widget->onMouseRelease(context);
        if (wasClick) return true;
    }

    return false;
}

bool Widget::onMouseMove(plugin::MouseContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];

        if (widget->getAvailable()) wasClick = widget->onMouseMove(context);
        if (wasClick) return true;
    }

    return false;
}

bool Widget::isInside(MPoint point) {
    return MathRectangle(position, size).isPointInside(point);
}

RegionSet* Widget::getDefaultRegSet() {
    createEmptyRegionSet();
    return regSet;
}


void Widget::registerObject(Widget* widget) {
    ON_ERROR(!widget, "Widget ptr was null!",)

    widget->setParent(this);
    subWindows->pushBack((Widget*)widget);

    fillRegionSets();  
}

void Widget::unregisterObject() {
    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];
        if (!widget->getAvailable()) {
            fprintf(stderr, "CALL DEST: %s\n", typeid(*widget).name());
            delete widget;

            subWindows->remove(i);
            listSize--;

            fillRegionSets();
        }
    }
}

void Widget::render(RenderTarget* renderTarget) {
    unregisterObject();

    if (visible && getAvailable()) {
        size_t listSize = subWindows->getSize();
        for (size_t i = 0; i < listSize; i++) {
            Widget* widget = (*subWindows)[i];
            widget->render(renderTarget);
        }

        // regSet->visualize(renderTarget, debColor);
    }
}

void Widget::clearRegionSets() {
    size_t listSize = subWindows->getSize();

    regSet->getRectangles()->clear();

    for (size_t i = 0; i < listSize; i++) {
        Widget* widget_ptr = (*subWindows)[i];

        widget_ptr->clearRegionSets();
    }
}

void Widget::fillRegionSets() {
    Widget* test = this;

    while (test->parent) test = test->parent;

    test->fillRegionSetsRoot();
}

void Widget::fillRegionSetsRoot() {
    if (!regSet || !regSet->getRectangles()) return;

    regSet = getDefaultRegSet();

    if (parent) {
        RegionSet* oldRegSet = regSet;
        regSet = regSet->cross(parent->regSet);
        delete oldRegSet;

        List<Widget*>* parentWins = parent->subWindows;
        size_t           parentCCnt = parentWins->getSize();
        size_t           valInd     = 0;
        for (size_t i = 0; i < parentCCnt; i++) {
            Widget* widget_ptr = (*parentWins)[i];
            if (this == widget_ptr) {
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
        Widget* subWin = (*subWindows)[i];
        if (subWin->visible) 
            subWin->recalcRegion();
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
    Widget* test_parent = parent;

    if (test_parent && test_parent->getWindows()) {
        test_parent->getWindows()->swapWithEnd(this);
        test_parent->fillRegionSets();
    }
}