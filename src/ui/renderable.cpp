#include "renderable.h"

WidgetPtr::WidgetPtr() {}

WidgetPtr::WidgetPtr(plugin::WidgetI* _widget) {
    is_extern = true;
    plugin_widget = _widget;
}

WidgetPtr::WidgetPtr(Widget* _widget) {
    is_extern      = false;
    program_widget = _widget;
}

plugin::Vec2 WidgetPtr::getSize() {
    if (is_extern) return plugin_widget->getSize();
    return program_widget->getSize();
}
plugin::Vec2 WidgetPtr::getPos() {
    if (is_extern) return plugin_widget->getPos();
    return program_widget->getPos();
}

bool WidgetPtr::onKeyboardPress(plugin::KeyboardContext key) {
    if (is_extern) return plugin_widget->onKeyboardPress(key);
    return program_widget->onKeyboardPress(key);
}

bool WidgetPtr::onKeyboardRelease(plugin::KeyboardContext key) {
    if (is_extern) return plugin_widget->onKeyboardRelease(key);
    return program_widget->onKeyboardRelease(key);
}

bool WidgetPtr::onMousePress(plugin::MouseContext mouse) {
    if (is_extern) return plugin_widget->onMousePress(mouse);
    return program_widget->onMousePress(mouse);
}

bool WidgetPtr::onMouseRelease(plugin::MouseContext mouse) {
    if (is_extern) return plugin_widget->onMouseRelease(mouse);
    return program_widget->onMouseRelease(mouse);
}

bool WidgetPtr::onMouseMove(plugin::MouseContext mouse) {
    if (is_extern) return plugin_widget->onMouseMove(mouse);
    return program_widget->onMouseMove(mouse);
}

bool WidgetPtr::onClock(size_t delta) {
    if (is_extern) return plugin_widget->onClock(delta);
    return program_widget->onClock(delta);
}

void WidgetPtr::move(plugin::Vec2 shift) {
    if (is_extern) plugin_widget ->move(shift);
    else           program_widget->move(shift);
}

bool WidgetPtr::isInside(plugin::Vec2 position) {
    if (is_extern) {
        MathRectangle widget_rect = MathRectangle(MPoint(plugin_widget->getPos()),
                                                  MPoint(plugin_widget->getSize()));
        return widget_rect.isPointInside(MPoint(position));
    }

    return program_widget->isInside(MPoint(position));
}

void WidgetPtr::setAvailable(bool value) {
    if (is_extern) plugin_widget ->setAvailable(value);
    else           program_widget->setAvailable(value);
}

bool WidgetPtr::getAvailable() {
    if (is_extern) plugin_widget->getAvailable();
    return program_widget->getAvailable();
}

bool operator==(const WidgetPtr& a, const WidgetPtr& b) {
    if (!a.is_extern && !b.is_extern) return a.program_widget == b.program_widget;
    return (a.is_extern && b.is_extern) && (a.plugin_widget == b.plugin_widget);
}

Widget::Widget(MPoint _position, MPoint _size, Widget* _parent, uint8_t _priority) :
    position(_position),
    size    (_size),
    parent  (_parent),
    visible (true),
    exists  (true),
    priority(_priority) {
        debColor = MColor(DEB_COLS[rand() % DEB_COLS_CNT]);
        subWindows = new List<WidgetPtr>();
        createEmptyRegionSet();
    }

Widget::Widget(MPoint _position, MPoint _size, Widget* _parent, List<WidgetPtr>* _subWindows, uint8_t _priority) :
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
        WidgetPtr widget = (*subWindows)[i];
        if (!widget.is_extern) delete widget.program_widget;
    }

    delete subWindows;
    subWindows = nullptr;

    delete regSet;
    regSet = nullptr;
}

void Widget::registerSubWidget(plugin::WidgetI* object) {
    ON_ERROR(!object, "Widget ptr was null!",)

    object->setParent(this);
    subWindows->pushBack(WidgetPtr(object));
}

void Widget::unregisterSubWidget(plugin::WidgetI* object) {

}

plugin::Vec2 Widget::getSize() {
    return size.toVec2();
}

void Widget::setSize(plugin::Vec2 _size) {
    size = MPoint(_size);
}

plugin::Vec2 Widget::getPos() {
    return position.toVec2();
}

void Widget::setPos(plugin::Vec2 _pos) {
    position = MPoint(_pos);
}

bool Widget::isExtern() {
    return false;
}

void Widget::setParent(WidgetI *root) {
    parent = (Widget*) root;
}

plugin::WidgetI* Widget::getParent() {
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
        WidgetPtr widget = (*subWindows)[i];

        if (widget.getAvailable()) {
            widget.move(shift);
        }
    }
}

bool Widget::getAvailable() {
    return exists;
}

void Widget::setAvailable(bool _available) {
    exists = _available;
}

void Widget::render(plugin::RenderTargetI* rt) {

}

void Widget::recalcRegion() {

}

uint8_t Widget::getPriority() {
    return priority;
}

List<WidgetPtr>* Widget::getWindows() {
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
        WidgetPtr widget = (*subWindows)[i];

        if (widget.getAvailable()) {
            wasClick = widget.onKeyboardPress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool Widget::onKeyboardRelease(plugin::KeyboardContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*subWindows)[i];

        if (widget.getAvailable()) widget.onKeyboardRelease(context);
    }

    return true;
}

bool Widget::onClock(uint64_t delta) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*subWindows)[i];

        if (widget.getAvailable()) widget.onClock(delta);
    }

    return true;
}

bool Widget::onMousePress(plugin::MouseContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    bool wasClick = false;

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*subWindows)[i];

        if (widget.getAvailable() && widget.isInside(context.position)) {
            wasClick = widget.onMousePress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool Widget::onMouseRelease(plugin::MouseContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*subWindows)[i];

        if (widget.getAvailable()) widget.onMouseRelease(context);
    }

    return true;
}

bool Widget::onMouseMove(plugin::MouseContext context) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*subWindows)[i];

        if (widget.getAvailable()) widget.onMouseMove(context);
    }

    return true;
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
    subWindows->pushBack(WidgetPtr(widget));

    fillRegionSets();  
}

void Widget::unregisterObject() {
    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*subWindows)[i];
        if (!widget.getAvailable() && !widget.is_extern) {
            delete widget.program_widget;
            subWindows->remove(i);
            listSize--;

            fillRegionSets();
        }
    }
}

void Widget::render(RenderTarget* renderTarget) {
    unregisterObject();

    if (visible) {
        size_t listSize = subWindows->getSize();
        for (size_t i = 0; i < listSize; i++) {
            WidgetPtr widget = (*subWindows)[i];
            if (!widget.is_extern) {
                widget.program_widget->render(renderTarget);
            }
        }

        // regSet->visualize(renderTarget, debColor);
    }
}

void Widget::clearRegionSets() {
    size_t listSize = subWindows->getSize();

    regSet->getRectangles()->clear();

    for (size_t i = 0; i < listSize; i++) {
        WidgetPtr widget_ptr = (*subWindows)[i];

        if (!widget_ptr.is_extern) widget_ptr.program_widget->clearRegionSets();
    }
}

void Widget::fillRegionSets() {
    Widget* test = this;

    while (test->parent && !test->parent->isExtern()) test = test->parent;

    test->fillRegionSetsRoot();
}

void Widget::fillRegionSetsRoot() {
    if (!regSet || !regSet->getRectangles()) return;

    regSet = getDefaultRegSet();

    if (parent) {
        RegionSet* oldRegSet = regSet;
        regSet = regSet->cross(parent->regSet);
        delete oldRegSet;

        List<WidgetPtr>* parentWins = parent->subWindows;
        size_t           parentCCnt = parentWins->getSize();
        size_t           valInd     = 0;
        for (size_t i = 0; i < parentCCnt; i++) {
            WidgetPtr widget_ptr = (*parentWins)[i];
            if (!widget_ptr.is_extern && this == widget_ptr.program_widget) {
                valInd = i + 1;
                break;
            }
        }

        for (size_t i = valInd; i < parentCCnt; i++) {
            WidgetPtr cur = (*parentWins)[i];

            if (!cur.is_extern && cur.program_widget->visible) {
                regSet->subtract(cur.program_widget->getDefaultRegSet());
            }
        }
    }

    size_t childCnt = subWindows->getSize();

    for (size_t i = 0; i < childCnt; i++) {
        WidgetPtr subWin = (*subWindows)[i];
        if (!subWin.is_extern && subWin.program_widget->visible) subWin.program_widget->fillRegionSetsRoot();
    }

    // minus children
    for (size_t i = 0; i < childCnt; i++) {
        WidgetPtr cur = (*subWindows)[i];

        if (!cur.is_extern && cur.program_widget->visible) {
            regSet->subtract(cur.program_widget->getDefaultRegSet());
        }
    }
}

void Widget::prioritizeWindow() {
    Widget* test_parent = parent;

    if (test_parent && test_parent->getWindows()) {
        test_parent->getWindows()->swapWithEnd(WidgetPtr(this));
        test_parent->fillRegionSets();
    }
}