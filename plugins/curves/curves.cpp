#include "curves.h"

using namespace ThreadJava800_Curve_FIlter;

plugin::Plugin* getInstance(plugin::App* app) {
    return new CurveFilter(app);
}

DefaultWidget::DefaultWidget(plugin::App* _app) {
    children = new ThreadJava800_List::List<DefaultWidget*>();

    _app->event_manager->registerObject(this);

    _app->event_manager->setPriority(plugin::EventType::MousePress, 1);
    _app->event_manager->setPriority(plugin::EventType::MouseMove, 1);
    _app->event_manager->setPriority(plugin::EventType::MouseRelease, 1);
}

void DefaultWidget::registerSubWidget(plugin::WidgetI* object) {
    object->setParent(this);

    (*children).pushBack((DefaultWidget*)object);
}

void DefaultWidget::unregisterSubWidget(plugin::WidgetI* object) {
    for (size_t i = 0; i < (*children).getSize(); i++) {
        if (object == (*children)[i]) {
            (*children).remove(i);
            break;
        }
    }
}

plugin::Vec2 DefaultWidget::getSize() {
    return size;
}

void DefaultWidget::setSize(plugin::Vec2 _size) {
    size = _size;
}

plugin::Vec2 DefaultWidget::getPos() {
    return position;
}

void DefaultWidget::setPos(plugin::Vec2 _pos) {
    position = _pos;
}

bool DefaultWidget::isExtern() {
    return true;
}

void DefaultWidget::setParent(WidgetI *root) {
    parent = root;
}

plugin::WidgetI* DefaultWidget::getParent() {
    return parent;
}

void DefaultWidget::move(plugin::Vec2 shift) {
    size_t listSize = (*children).getSize();
    position = {position.x + shift.x, position.y + shift.y};

    for (size_t i = 0; i < listSize; i++) {
        plugin::WidgetI* widget = (*children)[i];

        if (widget->getAvailable()) {
            widget->move(shift);
        }
    }
}

bool DefaultWidget::getAvailable() {
    return is_alive;
}

void DefaultWidget::setAvailable(bool _available) {
    is_alive = _available;
}

void DefaultWidget::render(plugin::RenderTargetI* rt) {
    if (is_alive) {
        size_t list_size = (*children).getSize();
        for (size_t i = 0; i < list_size; i++) {
            DefaultWidget* widget = (*children)[i];
            widget->render(rt);
        }
    }
}

void DefaultWidget::recalcRegion() {}

bool DefaultWidget::onMouseMove(plugin::MouseContext context) {
    long list_size = long((*children).getSize());
    for (long i = list_size - 1; i >= 0; i--) {
        plugin::WidgetI* widget = (*children)[i];

        if (widget && widget->getAvailable()) widget->onMouseMove(context);
    }

    return true;
}

bool DefaultWidget::onMouseRelease(plugin::MouseContext context) {
    long list_size = long((*children).getSize());
    for (long i = list_size - 1; i >= 0; i--) {
        plugin::WidgetI* widget = (*children)[i];

        if (widget->getAvailable()) widget->onMouseRelease(context);
    }

    return true;
}

bool DefaultWidget::onMousePress(plugin::MouseContext context) {
    bool was_click = false;

    long list_size = long((*children).getSize());
    for (long i = list_size - 1; i >= 0; i--) {
        DefaultWidget* widget = (*children)[i];
        if (widget->getAvailable()) {
            was_click = widget->onMousePress(context);
            if (was_click) return was_click;
        }
    }

    return was_click;
}

bool DefaultWidget::onKeyboardPress(plugin::KeyboardContext context) {
    return false;
}

bool DefaultWidget::onKeyboardRelease(plugin::KeyboardContext context) {
    return false;
}

bool DefaultWidget::onClock(uint64_t delta) {
    return false;
}

uint8_t DefaultWidget::getPriority() {
    return 1;
}

bool DefaultWidget::isInside(plugin::Vec2 pos) {
    return (position.x <= pos.x && pos.x <= position.x + size.x) &&
           (position.y <= pos.y && pos.y <= position.y + size.y);
}

DefaultWidget::~DefaultWidget() {
    delete children;
}

TopPanel::TopPanel(plugin::App* _app, plugin::Color _color, plugin::Vec2 _position, int width) : DefaultWidget(_app) {
    size     = {(double)width, TOP_PANE_SIZE};
    position = _position;
    color    = _color;
}

void TopPanel::render(plugin::RenderTargetI* rt) {
    rt->drawRect(position, size, color);
}

bool TopPanel::onMousePress(plugin::MouseContext context) {
    if (!isInside(context.position)) return false;

    is_clicked = true;
    last_pos   = context.position;

    return true;
}

bool TopPanel::onMouseRelease(plugin::MouseContext context) {
    is_clicked = false;

    return true;
}

bool TopPanel::onMouseMove(plugin::MouseContext context) {
    if (is_clicked) {
        parent->move({context.position.x - last_pos.x, context.position.y - last_pos.y});
        last_pos = context.position;

        return true;
    }

    return false;
}

void CurveWindow::drawFrame(plugin::RenderTargetI* rt, plugin::Color color) {
    rt->drawLine(position, {position.x, position.y + size.y}, color);
    rt->drawLine(position, {position.x + size.x, position.y}, color);
    rt->drawLine({position.x + size.x, position.y}, {position.x + size.x, position.y + size.y}, color);
    rt->drawLine({position.x, position.y + size.y}, {position.x + size.x, position.y + size.y}, color);
}

CurveWindow::CurveWindow(plugin::App* _app, const char* _window_name) : DefaultWidget(_app), app(_app) {
    window_name = strdup(_window_name);

    plugin::Vec2 root_size = app->root->getSize();

    size     = {root_size.x / 2, root_size.y / 2};
    position = {root_size.x / 4, root_size.y / 4};

    TopPanel* top_panel = new TopPanel(app, LIGHT_BLUE, position, size.x);
    registerSubWidget(top_panel);
}

void CurveWindow::render(plugin::RenderTargetI* rt) {
    rt->drawRect(position, size, WHITE);

    DefaultWidget::render(rt);

    drawFrame(rt, GRAY);
    rt->drawText({position.x + 2 * TOP_PANE_SIZE, position.y}, window_name, BTN_TXT_PT, BLACK);
}

CurveWindow::~CurveWindow() {
    if (window_name) free(window_name);
}

CurveFilter::CurveFilter(plugin::App* _app) : app(_app) {
    id   = 4;
    name = "SegCurve";
    type = plugin::InterfaceType::Filter;
}

void CurveFilter::apply(plugin::RenderTargetI *data) {
    if (!win_created) {
        app->root->getRoot()->registerSubWidget(new CurveWindow(app, "Test CurveWindow"));
        win_created = true;
    }
}

plugin::Array<const char *> CurveFilter::getParamNames() {
    plugin::Array<const char*> arr;
    arr.size = 0;

    return arr;
}

plugin::Array<double> CurveFilter::getParams() {
    plugin::Array<double> arr;
    arr.size = 0;

    return arr;
}

void CurveFilter::setParams(plugin::Array<double> params) {}

plugin::Interface* CurveFilter::getInterface () {
    return this;
}