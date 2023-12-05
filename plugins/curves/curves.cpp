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

DefaultWidget::DefaultWidget(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size) :
    position(_pos), size(_size) {

    children = new ThreadJava800_List::List<DefaultWidget*>();

    _app->event_manager->registerObject(this);

    _app->event_manager->setPriority(plugin::EventType::MousePress, 1);
    _app->event_manager->setPriority(plugin::EventType::MouseMove, 1);
    _app->event_manager->setPriority(plugin::EventType::MouseRelease, 1);
}

bool DefaultWidget::getVisible() {
    return is_visible;
}

void DefaultWidget::setVisible(bool _vis) {
    is_visible = _vis;
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
            if (widget->getVisible()) widget->render(rt);
        }
    }
}

void DefaultWidget::recalcRegion() {}

bool DefaultWidget::onMouseMove(plugin::MouseContext context) {
    long list_size = long((*children).getSize());
    for (long i = list_size - 1; i >= 0; i--) {
        DefaultWidget* widget = (*children)[i];

        if (widget->getAvailable() && widget->getVisible()) widget->onMouseMove(context);
    }

    return true;
}

bool DefaultWidget::onMouseRelease(plugin::MouseContext context) {
    long list_size = long((*children).getSize());
    for (long i = list_size - 1; i >= 0; i--) {
        DefaultWidget* widget = (*children)[i];

        if (widget->getAvailable() && widget->getVisible()) widget->onMouseRelease(context);
    }

    return true;
}

bool DefaultWidget::onMousePress(plugin::MouseContext context) {
    bool was_click = false;

    long list_size = long((*children).getSize());
    for (long i = list_size - 1; i >= 0; i--) {
        DefaultWidget* widget = (*children)[i];
        if (widget->getAvailable() && widget->getVisible()) {
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

OnCloseClick::OnCloseClick(DefaultWidget* _widget) : widget(_widget) {}

void OnCloseClick::operator()() {
    widget->setAvailable(false);
}

OnTabChangeClick::OnTabChangeClick(CurveWindow* _curve_win, CurveWindow::ACTIVE_SUB_WIN _change_to) : 
    curve_win(_curve_win), change_to_win(_change_to) {}

void OnTabChangeClick::operator()() {
    curve_win->setActiveTab(change_to_win);
}

Button::Button(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, OnClick* _on_click) : 
    DefaultWidget(_app, _pos, _size), on_click(_on_click) {}

bool Button::onMousePress(plugin::MouseContext context) {
    if (isInside(context.position) && on_click) {
        if (on_click) {
            (*on_click)();
            return true;
        }
    }
    return false;
}

Button::~Button() {
    if(on_click) delete on_click;
}

TextButton::TextButton(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, OnClick* _on_click, const char* _text) : 
    Button(_app, _pos, _size, _on_click) {
    text = strdup(_text);
}

TextButton::~TextButton() {
    if (text) free(text);
}

void TextButton::render(plugin::RenderTargetI* rt) {
    rt->drawRect(position, size, WHITE);
    rt->drawText(position, text, BTN_TXT_PT, BLACK);
}

TopPanel::TopPanel(plugin::App* _app, plugin::Color _color, plugin::Vec2 _position, int width) : DefaultWidget(_app) {
    size     = {(double)width, TOP_PANE_SIZE};
    position = _position;
    color    = _color;
}

void TopPanel::render(plugin::RenderTargetI* rt) {
    rt->drawRect(position, size, color);
    DefaultWidget::render(rt);
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

size_t CurvePolyLine::addPoint(plugin::Vec2 point) {
    size_t ret_ind = -1;
    for (size_t i = 0; i < points.getSize() - 1; i++) {
        if (points[i].x < point.x && point.x < points[i + 1].x) {
            points.insert(point, i + 1);
            ret_ind = i + 1;
            break;
        }
    }
    return ret_ind;
}

bool CurvePolyLine::isPointOnLine(plugin::Vec2 line_point1, plugin::Vec2 line_point2, plugin::Vec2 check_point) {
    double vec_len  = sqrt((line_point2.y - line_point1.y) * (line_point2.y - line_point1.y) + (line_point2.x - line_point1.x) * (line_point2.x - line_point1.x));
    double distance = abs((line_point2.y - line_point1.y) * check_point.x - (line_point2.x - line_point1.x) * check_point.y + line_point2.x * line_point1.y - line_point2.y * line_point1.x) / vec_len;

    return distance < LINE_SHIFT;
}

bool CurvePolyLine::areSamePoints(plugin::Vec2 point1, plugin::Vec2 point2) {
    double distance = sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
    return distance < LINE_SHIFT;
}

size_t CurvePolyLine::trySwapPoint(size_t point) {
    if (points[point].x < points[point - 1].x) {
        std::swap(points[point], points[point - 1]);
        return point - 1;
    }
    if (points[point].x > points[point + 1].x) {
        std::swap(points[point], points[point + 1]);
        return point + 1;
    }

    return point;
}

CurvePolyLine::CurvePolyLine(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, CurveCoordPlane* _coord_plane, plugin::Color _color) :
    DefaultWidget(_app, _pos, _size), coord_plane(_coord_plane), line_color(_color) {
    points.pushBack({position.x, position.y + size.y});
    points.pushBack({position.x + size.x, position.y});
}

bool CurvePolyLine::onMousePress(plugin::MouseContext context) {
    if (isInside(context.position)) {
        is_active    = true;

        for (size_t i = 1; i < points.getSize() - 1; i++) {
            if (areSamePoints(points[i], context.position)) active_point = i;
        }
        if (active_point == -1) active_point = addPoint(context.position);

        return true;
    }
    return false;
}

bool CurvePolyLine::onMouseRelease(plugin::MouseContext context) {
    is_active    = false;
    active_point = -1;

    return true;
}

bool CurvePolyLine::onMouseMove(plugin::MouseContext context) {
    if (is_active && active_point >= 0) {
        points[active_point] = context.position;
        active_point = trySwapPoint(active_point);
        return true;
    }
    return false;
}

void CurvePolyLine::move(plugin::Vec2 shift) {
    for (size_t i = 0; i < points.getSize(); i++) {
        points[i].x += shift.x;
        points[i].y += shift.y;
    }
}

void CurvePolyLine::render(plugin::RenderTargetI* rt) {
    for (int i = 0; i < points.getSize() - 1; i++) {
        if (i != 0) {
            rt->drawRect(points[i], {10, 10}, line_color);
        }
        rt->drawLine(points[i], points[i + 1], line_color);
    }
}

bool CurvePolyLine::isInside(plugin::Vec2 pos) {
    for (size_t i = 0; i < points.getSize() - 1; i++) {
        if (isPointOnLine(points[i], points[i + 1], pos)) return true;
    }
    return false;
}

CurveCoordPlane::CurveCoordPlane(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, plugin::Vec2 _unit, plugin::Vec2 _max_unit) :
    DefaultWidget(_app, _pos, _size), unit(_unit), max_unit(_max_unit) {

}

void CurveCoordPlane::render(plugin::RenderTargetI* rt) {
    // draw vertical axis
    int shift_x = size.x * unit.x / max_unit.x;
    for (int i = 0; i <= max_unit.x / unit.x; i++) {
        rt->drawLine({position.x + i * shift_x, position.y + size.y - SHIFT_FROM_EDGES.y}, {position.x + i * shift_x, position.y}, BLACK);

        char axis_txt[MAX_UNIT_LEN] = "";
        sprintf(axis_txt, "%d", i * int(unit.x));
        rt->drawText({position.x + i * shift_x, position.y + size.y}, axis_txt, BTN_TXT_PT, BLACK);
    }

    // draw horizontal
    int shift_y = size.y * unit.y / max_unit.y;
    for (int i = 0; i <= max_unit.y / unit.y; i++) {
        rt->drawLine({SHIFT_FROM_EDGES.x + position.x, position.y + size.y - i * shift_y}, {position.x + size.x, position.y + size.y - i * shift_y}, BLACK);
        
        char axis_txt[MAX_UNIT_LEN] = {};
        sprintf(axis_txt, "%d", (int(max_unit.y / unit.y) - i) * int(unit.y));
        rt->drawText({position.x - 5 * SHIFT_FROM_EDGES.x, position.y + i * shift_y - SHIFT_FROM_EDGES.y}, axis_txt, BTN_TXT_PT, BLACK);
    }

    DefaultWidget::render(rt);
}

void CurveWindow::drawFrame(plugin::RenderTargetI* rt, plugin::Color color) {
    rt->drawLine(position, {position.x, position.y + size.y}, color);
    rt->drawLine(position, {position.x + size.x, position.y}, color);
    rt->drawLine({position.x + size.x, position.y}, {position.x + size.x, position.y + size.y}, color);
    rt->drawLine({position.x, position.y + size.y}, {position.x + size.x, position.y + size.y}, color);
}

void CurveWindow::createTopPanel() {
    // panel with close button
    TopPanel* top_panel = new TopPanel(app, LIGHT_BLUE, position, size.x);

    TextButton* on_close = new TextButton(app, position, {TOP_PANE_SIZE, TOP_PANE_SIZE}, new OnCloseClick(this), "X");
    top_panel->registerSubWidget(on_close);

    registerSubWidget(top_panel);

    // tab section
    plugin::Vec2 tab_size = {size.x / 3, TOP_PANE_SIZE};
    TextButton* red_tab   = new TextButton(app, {position.x                 , position.y + TOP_PANE_SIZE}, tab_size, new OnTabChangeClick(this, ACTIVE_SUB_WIN::RED_WIN), "Red");
    TextButton* green_tab = new TextButton(app, {position.x +     tab_size.x, position.y + TOP_PANE_SIZE}, tab_size, new OnTabChangeClick(this, ACTIVE_SUB_WIN::GREEN_WIN), "Green");
    TextButton* blue_tab  = new TextButton(app, {position.x + 2 * tab_size.x, position.y + TOP_PANE_SIZE}, tab_size, new OnTabChangeClick(this, ACTIVE_SUB_WIN::BLUE_WIN), "Blue");

    registerSubWidget(red_tab);
    registerSubWidget(green_tab);
    registerSubWidget(blue_tab);

    // create coord planes
    plugin::Vec2 plane_size = {std::min(size.x - 4 * TOP_PANE_SIZE, size.y - 4 * TOP_PANE_SIZE), std::min(size.x - 4 * TOP_PANE_SIZE, size.y - 4 * TOP_PANE_SIZE)};
    plugin::Vec2 plane_pos  = {position.x + (size.x - plane_size.x) / 2, position.y + (size.y - plane_size.y) / 2};

    CurveCoordPlane* red_plane = new CurveCoordPlane(app, plane_pos, plane_size, {51, 51}, {255, 255});
    CurvePolyLine  * red_line  = new CurvePolyLine  (app, plane_pos, plane_size, red_plane, RED);

    red_plane->registerSubWidget(red_line);
    registerSubWidget(red_plane);
}

CurveWindow::CurveWindow(plugin::App* _app, const char* _window_name) : DefaultWidget(_app), app(_app) {
    window_name = strdup(_window_name);

    plugin::Vec2 root_size = app->root->getSize();

    size     = {root_size.x / 2, root_size.y / 2};
    position = {root_size.x / 4, root_size.y / 4};

    createTopPanel();
}

void CurveWindow::render(plugin::RenderTargetI* rt) {
    rt->drawRect(position, size, WHITE);

    DefaultWidget::render(rt);

    drawFrame(rt, GRAY);
    rt->drawText({position.x + 2 * TOP_PANE_SIZE, position.y}, window_name, BTN_TXT_PT, BLACK);
}

void CurveWindow::setActiveTab(ACTIVE_SUB_WIN _new_win) {
    active_win = _new_win;
}

CurveWindow::~CurveWindow() {
    if (window_name) free(window_name);

    app->event_manager->setPriority(plugin::EventType::MousePress, 0);
    app->event_manager->setPriority(plugin::EventType::MouseMove, 0);
    app->event_manager->setPriority(plugin::EventType::MouseRelease, 0);

    app->event_manager->unregisterObject(this);
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