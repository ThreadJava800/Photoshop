#include "pluginWidget.h"

void PluginWidget::createDefaultRegSet() {
    if (reg_set) delete reg_set;

    reg_set = new RegionSet();
    reg_set->addRegion(MathRectangle(MPoint(position), MPoint(size)));
}

PluginWidget::PluginWidget() {
    children = new List<plugin::WidgetI*>();
    createDefaultRegSet();
}
PluginWidget::PluginWidget(plugin::Vec2 _pos, plugin::Vec2 _size) :
    position(_pos), size(_size) {
    children = new List<plugin::WidgetI*>();
    createDefaultRegSet();
}

void PluginWidget::registerSubWidget(plugin::WidgetI* object) {
    children->pushBack(object);
}

void PluginWidget::unregisterSubWidget(plugin::WidgetI* object) {
    size_t children_cnt = children->getSize();
    for (size_t i = 0; i < children_cnt; i++) {
        if ((*children)[i] == object) children->remove(i);
    }
}

plugin::Vec2 PluginWidget::getSize() {
    return size;
}
void PluginWidget::setSize(plugin::Vec2 _size) {
    size = _size;
}

plugin::Vec2 PluginWidget::getPos() {
    return position;
}

void PluginWidget::setPos(plugin::Vec2 _pos) {
    position = _pos;
}

bool PluginWidget::isExtern() {
    return true;
}

void PluginWidget::setParent(plugin::WidgetI *root) {
    parent = root;
}

plugin::WidgetI* PluginWidget::getParent() {
    return parent;
}

void PluginWidget::move(plugin::Vec2 shift) {
    position.x += shift.x;
    position.y += shift.y;
}

bool PluginWidget::getAvailable() {
    return is_available;
}

void PluginWidget::setAvailable(bool _is_available) {
    is_available = _is_available;
}

void PluginWidget::render(plugin::RenderTargetI* rend_target) {
    if (is_available) {
        size_t children_cnt = children->getSize();

        for (size_t i = 0; i < children_cnt; i++) {
            plugin::WidgetI* widget = (*children)[i];
            if (widget) widget->render(rend_target);
        }
    }
}

void PluginWidget::recalcRegion() {
    createDefaultRegSet();
}

PluginWidget::~PluginWidget() {
    if (children) delete children;
    if (reg_set)  delete reg_set;
}

bool PluginWidget::onMouseMove(plugin::MouseContext context) {
    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetI* widget = (*children)[i];

        if (widget && widget->isExtern()) widget->onMouseMove(context);
    }

    return true;
}

bool PluginWidget::onMouseRelease(plugin::MouseContext context) {
    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetI* widget = (*children)[i];

        if (widget && widget->isExtern()) widget->onMouseRelease(context);
    }

    return true;
}

bool PluginWidget::onMousePress(plugin::MouseContext context) {
    bool wasClick = false;

    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetI* widget = (*children)[i];

        if (widget && widget->isExtern()) {
            wasClick = widget->onMousePress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool PluginWidget::onKeyboardPress(plugin::KeyboardContext context) {
    bool wasClick = false;

    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetI* widget = (*children)[i];

        if (widget && widget->isExtern()) {
            wasClick = widget->onKeyboardPress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool PluginWidget::onKeyboardRelease(plugin::KeyboardContext context) {
    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetI* widget = (*children)[i];

        if (widget && widget->isExtern()) widget->onKeyboardRelease(context);
    }

    return true;
}

bool PluginWidget::onClock(uint64_t delta) {
    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetI* widget = (*children)[i];

        if (widget && widget->isExtern()) widget->onClock(delta);
    }

    return true;
}