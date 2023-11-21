#include "pluginWidget.h"

WidgetPtr::WidgetPtr(plugin::WidgetI* _widget) {
    is_extern = true;
    plugin_widget = _widget;
}

WidgetPtr::WidgetPtr(Widget* _widget, RenderTarget* _program_render_target) {
    is_extern             = false;
    program_widget        = _widget;
    program_render_target = _program_render_target;
}

plugin::Vec2 WidgetPtr::getSize() {
    if (is_extern) return plugin_widget->getSize();
    return program_widget->getSize().toVec2();
}
plugin::Vec2 WidgetPtr::getPos() {
    if (is_extern) return plugin_widget->getPos();
    return program_widget->getPosition().toVec2();
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

void WidgetPtr::setAvailable(bool value) {
    if (is_extern) plugin_widget ->setAvailable(value);
    else           program_widget->setExists(value);
}

bool WidgetPtr::getAvailable() {
    if (is_extern) plugin_widget->getAvailable();
    return program_widget->getExists();
}

void WidgetPtr::render(plugin::RenderTargetI* rend_target) {
    if (is_extern) plugin_widget ->render(rend_target);
    else           program_widget->render(program_render_target);
}

void PluginWidget::createDefaultRegSet() {
    if (reg_set) delete reg_set;

    reg_set = new RegionSet();
    reg_set->addRegion(MathRectangle(MPoint(position), MPoint(size)));
}

PluginWidget::PluginWidget(uint8_t _priority) : priority(_priority) {
    children = new List<WidgetPtr>();
    createDefaultRegSet();
}
PluginWidget::PluginWidget(plugin::Vec2 _pos, plugin::Vec2 _size, uint8_t _priority) :
    position(_pos), size(_size), priority(_priority) {
    children = new List<WidgetPtr>();
    createDefaultRegSet();
}

void PluginWidget::registerSubWidget(plugin::WidgetI* object) {
    children->pushBack(WidgetPtr(object));
}

void PluginWidget::unregisterSubWidget(plugin::WidgetI* object) {
    size_t children_cnt = children->getSize();
    for (size_t i = 0; i < children_cnt; i++) {
        WidgetPtr child = (*children)[i];
        if (child.is_extern && child.plugin_widget == object) {
            children->remove(i);
            break;
        }
    }
}

void PluginWidget::registerSubWidget(Widget* object, RenderTarget* _objRendTarget) {
    children->pushBack(WidgetPtr(object, _objRendTarget));
}

void PluginWidget::unregisterSubWidget(Widget* object) {
    size_t children_cnt = children->getSize();
    for (size_t i = 0; i < children_cnt; i++) {
        WidgetPtr child = (*children)[i];
        if (!child.is_extern && child.program_widget == object) {
            children->remove(i);
            break;
        }
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
            WidgetPtr widget = (*children)[i];
            if (widget.getAvailable()) widget.render(rend_target);
        }
    }
}

void PluginWidget::recalcRegion() {
    createDefaultRegSet();
}

PluginWidget::~PluginWidget() {
    if (children)         delete children;
    if (reg_set)          delete reg_set;
}

bool PluginWidget::onMouseMove(plugin::MouseContext context) {
    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*children)[i];

        if (widget.getAvailable()) widget.onMouseMove(context);
    }

    return true;
}

bool PluginWidget::onMouseRelease(plugin::MouseContext context) {
    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*children)[i];

        if (widget.getAvailable()) widget.onMouseRelease(context);
    }

    return true;
}

bool PluginWidget::onMousePress(plugin::MouseContext context) {
    bool wasClick = false;

    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*children)[i];

        if (widget.getAvailable()) {
            wasClick = widget.onMousePress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool PluginWidget::onKeyboardPress(plugin::KeyboardContext context) {
    bool wasClick = false;

    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*children)[i];

        if (widget.getAvailable()) {
            wasClick = widget.onKeyboardPress(context);
            if (wasClick) return wasClick;
        }
    }

    return wasClick;
}

bool PluginWidget::onKeyboardRelease(plugin::KeyboardContext context) {
    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*children)[i];

        if (widget.getAvailable()) widget.onKeyboardRelease(context);
    }

    return true;
}

bool PluginWidget::onClock(uint64_t delta) {
    long listSize = static_cast<long>(children->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        WidgetPtr widget = (*children)[i];

        if (widget.getAvailable()) widget.onClock(delta);
    }

    return true;
}

uint8_t PluginWidget::getPriority() {
    return priority;
}