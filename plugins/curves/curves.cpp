#include "curves.h"

using namespace ThreadJava800_Curve_FIlter;

static double lerp(double t1, double t2, double t3) {
    return t1 + t3 * (t2 - t1);
}

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

OnTabChangeClick::OnTabChangeClick(CurveCoordPlane* _red_plane, TextButton* _red_tab, CurveCoordPlane* _green_plane, TextButton* _green_tab, CurveCoordPlane* _blue_plane, TextButton* _blue_tab, CurveWindow::ACTIVE_SUB_WIN _this_win) : 
    red_plane  (_red_plane),   red_tab  (_red_tab),
    green_plane(_green_plane), green_tab(_green_tab),
    blue_plane (_blue_plane),  blue_tab (_blue_tab),
    this_win(_this_win) {}

void OnTabChangeClick::operator()() {
    switch (this_win)
    {
    case CurveWindow::ACTIVE_SUB_WIN::RED_WIN:
        red_plane  ->setVisible(true);
        green_plane->setVisible(false);
        blue_plane ->setVisible(false);

        red_tab  ->setColor(LIGHT_BLUE);
        green_tab->setColor(WHITE);
        blue_tab ->setColor(WHITE);
        break;
    case CurveWindow::ACTIVE_SUB_WIN::GREEN_WIN:
        red_plane  ->setVisible(false);
        green_plane->setVisible(true);
        blue_plane ->setVisible(false);

        red_tab  ->setColor(WHITE);
        green_tab->setColor(LIGHT_BLUE);
        blue_tab ->setColor(WHITE);
        break;
    case CurveWindow::ACTIVE_SUB_WIN::BLUE_WIN:
        red_plane  ->setVisible(false);
        green_plane->setVisible(false);
        blue_plane ->setVisible(true);

        red_tab  ->setColor(WHITE);
        green_tab->setColor(WHITE);
        blue_tab ->setColor(LIGHT_BLUE);
        break;
    default:
        break;
    }
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

void TextButton::setOnClick(OnClick* _on_click) {
    on_click = _on_click;
}

void TextButton::setColor(plugin::Color _color) {
    color = _color;
}

void TextButton::render(plugin::RenderTargetI* rt) {
    rt->drawRect(position, size, color);
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

double CurvePolyLine::getCatmullCoeff(double prevCoeff, plugin::Vec2 p1, plugin::Vec2 p2) {
    plugin::Vec2 diffPoint = p2 - p1;

    return std::pow((diffPoint | diffPoint), CATMULL_ALPHA) + prevCoeff;
}

void CurvePolyLine::drawCatmullOf4(plugin::RenderTargetI* perm, plugin::Color color, plugin::Vec2 p0, plugin::Vec2 p1, plugin::Vec2 p2, plugin::Vec2 p3) {
    double t0 = 0;
    double t1 = getCatmullCoeff(t0, p0, p1);
    double t2 = getCatmullCoeff(t1, p1, p2);
    double t3 = getCatmullCoeff(t2, p2, p3);

    for (double i = 0; i < 1; i += 0.001) {
        double t  = lerp(t1, t2, i);

        if (t1 == t0 || t1 == t3 || t1 == t2 || t2 == t3 || t2 == t0) continue;

        plugin::Vec2 a1 = p0 * ((t1 - t) / (t1 - t0)) + p1 * ((t - t0) / (t1 - t0));
        plugin::Vec2 a2 = p1 * ((t2 - t) / (t2 - t1)) + p2 * ((t - t1) / (t2 - t1));
        plugin::Vec2 a3 = p2 * ((t3 - t) / (t3 - t2)) + p3 * ((t - t2) / (t3 - t2));

        plugin::Vec2 b1 = a1 * ((t2 - t) / (t2 - t0)) + a2 * ((t - t0) / (t2 - t0));
        plugin::Vec2 b2 = a2 * ((t3 - t) / (t3 - t1)) + a3 * ((t - t1) / (t3 - t1));

        plugin::Vec2 c  = b1 * ((t2 - t) / (t2 - t1)) + b2 * ((t - t1) / (t2 - t1));

        perm->drawEllipse(c, {LINE_DIAM, LINE_DIAM}, color);

        curve_points.pushBack(c);
    }
    drawCatmullOf3(perm, color, p1, p2, p3);
}

void CurvePolyLine::drawCatmullOf3(plugin::RenderTargetI* perm, plugin::Color color, plugin::Vec2 p1, plugin::Vec2 p2, plugin::Vec2 p3) {
    double t0 = 0;
    double t1 = getCatmullCoeff(t0, p1, p2);
    double t2 = getCatmullCoeff(t1, p2, p3);
    
    for (double i = 0; i <= 1; i += 0.001) {
        double t = lerp(t1, t2, i);

        plugin::Vec2 a1 = p1 * ((t1 - t) / (t1 - t0)) + p2 * ((t - t0) / (t1 - t0));
        plugin::Vec2 a2 = p2 * ((t2 - t) / (t2 - t1)) + p3 * ((t - t1) / (t2 - t1));
        plugin::Vec2 b  = a1 * ((t2 - t) / (t2 - t0)) + a2 * ((t - t0) / (t2 - t0));
        perm->drawEllipse(b, {LINE_DIAM, LINE_DIAM}, color);

        curve_points.pushBack(b);
    }
}

void CurvePolyLine::drawCatmullOf2(plugin::RenderTargetI* perm, plugin::Color color, plugin::Vec2 p1, plugin::Vec2 p2) {
    double t0 = 0;
    double t1 = getCatmullCoeff(t0, p1, p2);

    for (double i = 0; i <= 1; i += 0.001) {
        double t = lerp(t0, t1, i);
        plugin::Vec2 drawPnt = p1 * ((t1 - t) / (t1 - t0)) + p2 * ((t - t0) / (t1 - t0));
        perm->drawEllipse(drawPnt, {LINE_DIAM, LINE_DIAM}, color);

        curve_points.pushBack(drawPnt);
    }
}

void CurvePolyLine::drawCatmull(plugin::RenderTargetI* perm, plugin::Color color) {
    size_t pointCnt = points.getSize();
    curve_points.clear();

    if (pointCnt == 1) {
        perm->drawEllipse(points[0], {LINE_DIAM, LINE_DIAM}, color);
        curve_points.pushBack(points[0]);
        return;
    }
    if (pointCnt == 2) {
        drawCatmullOf2(perm, color, points[0], points[1]);
        return;
    }
    if (pointCnt == 3) {
        drawCatmullOf3(perm, color, points[0], points[1], points[2]);
        drawCatmullOf3(perm, color, points[2], points[1], points[0]);
        return;
    }

    for (size_t i = 0; i < pointCnt - 3; i++) {
        drawCatmullOf4(perm, color, points[i], points[i + 1], points[i + 2], points[i + 3]);
    }
    drawCatmullOf3(perm, color, points[2], points[1], points[0]);
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
    int min_y = std::min(line_point1.y, line_point2.y), max_y = std::max(line_point1.y, line_point2.y);
    int min_x = std::min(line_point1.x, line_point2.x), max_x = std::max(line_point1.x, line_point2.x);

    bool is_inside_rect = (min_y - LINE_SHIFT < check_point.y && check_point.y < max_y + LINE_SHIFT) &&
                          (min_x - LINE_SHIFT < check_point.x && check_point.x < max_x + LINE_SHIFT);
    if (!is_inside_rect) return false;

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

void CurvePolyLine::doApply(plugin::Texture* pl_texture, CurveWindow::ACTIVE_SUB_WIN change_col, uint8_t old_col, uint8_t new_col) {
    plugin::Vec2     texture_size = {pl_texture->width, pl_texture->height};

    for (uint64_t i = 0; i < texture_size.x; i++) {
        for (uint64_t j = 0; j < texture_size.y; j++) {
            plugin::Color new_pixel = pl_texture->pixels[i * (int)texture_size.y + j];

            switch (change_col) {
            case CurveWindow::ACTIVE_SUB_WIN::RED_WIN:
                if (new_pixel.r == old_col) new_pixel.r = new_col;
                break;
            case CurveWindow::ACTIVE_SUB_WIN::GREEN_WIN:
                if (new_pixel.g == old_col) new_pixel.g = new_col;
                break;
            case CurveWindow::ACTIVE_SUB_WIN::BLUE_WIN:
                if (new_pixel.b == old_col) new_pixel.b = new_col;
                break;
            default:
                break;
            }

            pl_texture->pixels[i * (int)texture_size.y + j] = new_pixel;
        }
    }
    data->drawTexture({0, 0}, texture_size, pl_texture);
}

plugin::Vec2 CurvePolyLine::getLocalCoord(plugin::Vec2 global_coord) {
    plugin::Vec2 max_unit = coord_plane->getMaxUnit();
    int x_coord = (global_coord.x - position.x) * max_unit.x / size.x;
    int y_coord = (size.y - (global_coord.y - position.y)) * max_unit.y / size.y;

    return {x_coord, y_coord};
}

CurvePolyLine::CurvePolyLine(plugin::RenderTargetI* _data, plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, CurveCoordPlane* _coord_plane, CurveWindow::ACTIVE_SUB_WIN _active_tab) :
    DefaultWidget(_app, _pos, _size), data(_data), coord_plane(_coord_plane), active_tab(_active_tab) {
    points.pushBack({position.x, position.y + size.y});
    points.pushBack({position.x + size.x, position.y});

    curve_points.pushBack({position.x, position.y + size.y});
    curve_points.pushBack({position.x + size.x, position.y});

    switch (active_tab) {
    case CurveWindow::ACTIVE_SUB_WIN::RED_WIN:
        line_color = RED;
        break;
    case CurveWindow::ACTIVE_SUB_WIN::GREEN_WIN:
        line_color = GREEN;
        break;
    case CurveWindow::ACTIVE_SUB_WIN::BLUE_WIN:
        line_color = BLUE;
        break;
    default:
        break;
    }
}

bool CurvePolyLine::onMousePress(plugin::MouseContext context) {
    if (!coord_plane->getVisible()) return false;

    if (isInside(context.position)) {
        is_active    = true;
        need_catmull = true;

        for (size_t i = 1; i < long(points.getSize()) - 1; i++) {
            if (areSamePoints(points[i], context.position)) active_point = i;
        }
        if (active_point == -1) active_point = addPoint(context.position);

        return true;
    }
    return false;
}

bool CurvePolyLine::onMouseRelease(plugin::MouseContext context) {
    if (is_active) {
        bool used[256];

        for (int i = 0; i < 256; i++) {
            used[i] = false;
        }

        plugin::Texture* pl_texture   = data->getTexture();

        for (size_t i = 0; i < curve_points.getSize(); i++) {
            plugin::Vec2 local_coord = getLocalCoord(curve_points[i]);

            int send_x = (int)local_coord.x;
            int send_y = (int)local_coord.y;

            if (!used [send_x]) {
                used  [send_x] = true;
                doApply(pl_texture, active_tab, send_x, send_y);
            }
        }

        delete[] pl_texture->pixels;
        delete   pl_texture;
        need_catmull = false;
    }

    is_active    = false;
    active_point = -1;

    return true;
}

bool CurvePolyLine::onMouseMove(plugin::MouseContext context) {
    if (!coord_plane->getVisible()) return false;

    if (is_active && active_point >= 0) {
        for (int i = 0; i < curve_points.getSize(); i++) {
            if (curve_points[i].x < position.x) {
                curve_points[i].x = position.x + LINE_SHIFT;
                return false;
            }
            if (position.x + size.x < curve_points[i].x) {
                curve_points[i].x = position.x + size.x - LINE_SHIFT;
                return false;
            }
            if (curve_points[i].y < position.y) {
                curve_points[i].y = position.y + LINE_SHIFT;
                return false;
            }
            if (position.y + size.y < curve_points[i].y) {
                curve_points[i].y = position.y + size.y - LINE_SHIFT;
                return false;
            }
        }

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
    for (size_t i = 0; i < curve_points.getSize(); i++) {
        curve_points[i].x += shift.x;
        curve_points[i].y += shift.y;
    }

    DefaultWidget::move(shift);
}

void CurvePolyLine::render(plugin::RenderTargetI* rt) {
    for (int i = 1; i < points.getSize() - 1; i++) {
        rt->drawRect(points[i], {10, 10}, line_color);
    }

    if (need_catmull) drawCatmull(rt, line_color);
    else {
        for (size_t i = 0; i < curve_points.getSize(); i += (curve_points.getSize() >> 9))
            rt->drawEllipse(curve_points[i], {LINE_DIAM, LINE_DIAM}, line_color);
    }
}

bool CurvePolyLine::isInside(plugin::Vec2 pos) {
    // for (size_t i = 0; i < curve_points.getSize() - 1; i++) {
    //     if (isPointOnLine(curve_points[i], curve_points[i + 1], pos)) return true;
    // }
    // return false;
    return DefaultWidget::isInside(pos);
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

plugin::Vec2 CurveCoordPlane::getMaxUnit() {
    return max_unit;
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
    red_tab   = new TextButton(app, {position.x                 , position.y + TOP_PANE_SIZE}, tab_size, nullptr, "Red");
    green_tab = new TextButton(app, {position.x +     tab_size.x, position.y + TOP_PANE_SIZE}, tab_size, nullptr, "Green");
    blue_tab  = new TextButton(app, {position.x + 2 * tab_size.x, position.y + TOP_PANE_SIZE}, tab_size, nullptr, "Blue");

    red_tab->setColor(LIGHT_BLUE);

    registerSubWidget(red_tab);
    registerSubWidget(green_tab);
    registerSubWidget(blue_tab);

    // create coord planes
    plugin::Vec2 plane_size = {std::min(size.x - 4 * TOP_PANE_SIZE, size.y - 4 * TOP_PANE_SIZE), std::min(size.x - 4 * TOP_PANE_SIZE, size.y - 4 * TOP_PANE_SIZE)};
    plugin::Vec2 plane_pos  = {position.x + (size.x - plane_size.x) / 2, position.y + (size.y - plane_size.y) / 2};

    CurveCoordPlane* red_plane = new CurveCoordPlane(app, plane_pos, plane_size, {51, 51}, {255, 255});
    CurvePolyLine  * red_line  = new CurvePolyLine  (data, app, plane_pos, plane_size, red_plane, CurveWindow::ACTIVE_SUB_WIN::RED_WIN);
    red_plane->registerSubWidget(red_line);
    registerSubWidget(red_plane);

    CurveCoordPlane* green_plane = new CurveCoordPlane(app, plane_pos, plane_size, {51, 51}, {255, 255});
    CurvePolyLine  * green_line  = new CurvePolyLine  (data, app, plane_pos, plane_size, green_plane, CurveWindow::ACTIVE_SUB_WIN::GREEN_WIN);
    green_plane->registerSubWidget(green_line);
    green_plane->setVisible(false);
    registerSubWidget(green_plane);

    CurveCoordPlane* blue_plane = new CurveCoordPlane(app, plane_pos, plane_size, {51, 51}, {255, 255});
    CurvePolyLine  * blue_line  = new CurvePolyLine  (data, app, plane_pos, plane_size, blue_plane, CurveWindow::ACTIVE_SUB_WIN::BLUE_WIN);
    blue_plane->registerSubWidget(blue_line);
    blue_plane->setVisible(false);
    registerSubWidget(blue_plane);

    red_tab  ->setOnClick(new OnTabChangeClick(red_plane, red_tab, green_plane, green_tab, blue_plane, blue_tab, CurveWindow::ACTIVE_SUB_WIN::RED_WIN));
    green_tab->setOnClick(new OnTabChangeClick(red_plane, red_tab, green_plane, green_tab, blue_plane, blue_tab, CurveWindow::ACTIVE_SUB_WIN::GREEN_WIN));
    blue_tab ->setOnClick(new OnTabChangeClick(red_plane, red_tab, green_plane, green_tab, blue_plane, blue_tab, CurveWindow::ACTIVE_SUB_WIN::BLUE_WIN));
}

CurveWindow::CurveWindow(plugin::RenderTargetI* _data, plugin::App* _app, const char* _window_name) : DefaultWidget(_app), app(_app), data(_data) {
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
        app->root->getRoot()->registerSubWidget(new CurveWindow(data, app, "Test CurveWindow"));
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