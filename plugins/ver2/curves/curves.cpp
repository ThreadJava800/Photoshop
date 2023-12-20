#include "curves.h"

using namespace ThreadJava800_Curve_FIlter;

static double lerp(double t1, double t2, double t3) {
    return t1 + t3 * (t2 - t1);
}

plugin::Plugin* getInstance(plugin::App* app) {
    return new CurveFilter(app);
}

DefaultWidget::DefaultWidget(plugin::App* _app) : app(_app) {
}

DefaultWidget::DefaultWidget(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size) :
    position(_pos), size(_size), app(_app) {
}

bool DefaultWidget::getVisible() {
    return is_visible;
}

void DefaultWidget::setVisible(bool _vis) {
    is_visible = _vis;
}

uint8_t DefaultWidget::getPriority() const {
    return 1;
}

bool DefaultWidget::isInside(plugin::Vec2 pos) {
    return (position.x <= pos.x && pos.x <= position.x + size.x) &&
           (position.y <= pos.y && pos.y <= position.y + size.y);
}

void DefaultWidget::move(plugin::Vec2 shift) {
    position += shift;
}

DefaultWidget::~DefaultWidget() {
    // size_t list_size = children->getSize();
    // for (size_t i = 0; i < list_size; i++) {
    //     DefaultWidget* widget = (*children)[i];
    //     delete widget;
    // }

    // delete children;
}

OnCloseClick::OnCloseClick(DefaultWidget* _widget) : widget(_widget) {}

void OnCloseClick::operator()() {
    widget->host->setAvailable(false);
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

        red_tab  ->setColor(LIGHT_GRAY);
        green_tab->setColor(WHITE);
        blue_tab ->setColor(WHITE);
        break;
    case CurveWindow::ACTIVE_SUB_WIN::GREEN_WIN:
        red_plane  ->setVisible(false);
        green_plane->setVisible(true);
        blue_plane ->setVisible(false);

        red_tab  ->setColor(WHITE);
        green_tab->setColor(LIGHT_GRAY);
        blue_tab ->setColor(WHITE);
        break;
    case CurveWindow::ACTIVE_SUB_WIN::BLUE_WIN:
        red_plane  ->setVisible(false);
        green_plane->setVisible(false);
        blue_plane ->setVisible(true);

        red_tab  ->setColor(WHITE);
        green_tab->setColor(WHITE);
        blue_tab ->setColor(LIGHT_GRAY);
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
    position = host->getPos();
    rt->drawRect(position, size, color);
    rt->drawText(position, text, BTN_TXT_PT, BLACK);
}

TopPanel::TopPanel(plugin::App* _app, plugin::Color _color, plugin::Vec2 _position, int width) : DefaultWidget(_app) {
    size     = {(double)width, TOP_PANE_SIZE};
    position = _position;
    color    = _color;
}

void TopPanel::render(plugin::RenderTargetI* rt) {
    position = host->getPos();
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
    if (is_clicked) {
        is_clicked = false;
        return true;
    }

    return false;
}

bool TopPanel::onMouseMove(plugin::MouseContext context) {
    if (is_clicked) {
        parent->host->move({context.position.x - last_pos.x, context.position.y - last_pos.y});
        last_pos = context.position;

        return true;
    }

    return false;
}

double CurvePolyLine::getCatmullCoeff(double prevCoeff, plugin::Vec2 p1, plugin::Vec2 p2) {
    plugin::Vec2 diffPoint = p2 - p1;

    return std::pow((diffPoint | diffPoint), CATMULL_ALPHA) + prevCoeff;
}

void CurvePolyLine::drawCatmullOf4(plugin::RenderTargetI* perm, plugin::Color color, plugin::Vec2 p0, plugin::Vec2 p1, plugin::Vec2 p2, plugin::Vec2 p3, bool drawing) {
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

        curve_points.pushBack(c);

        plugin::Vec2 loc_coord = getLocalCoord(c);
        draw_curve_points[(int)loc_coord.x] = loc_coord.y;
    }
    drawCatmullOf3(perm, color, p1, p2, p3, drawing);
}

void CurvePolyLine::drawCatmullOf3(plugin::RenderTargetI* perm, plugin::Color color, plugin::Vec2 p1, plugin::Vec2 p2, plugin::Vec2 p3, bool drawing) {
    double t0 = 0;
    double t1 = getCatmullCoeff(t0, p1, p2);
    double t2 = getCatmullCoeff(t1, p2, p3);
    
    for (double i = 0; i <= 1; i += 0.001) {
        double t = lerp(t1, t2, i);

        plugin::Vec2 a1 = p1 * ((t1 - t) / (t1 - t0)) + p2 * ((t - t0) / (t1 - t0));
        plugin::Vec2 a2 = p2 * ((t2 - t) / (t2 - t1)) + p3 * ((t - t1) / (t2 - t1));
        plugin::Vec2 b  = a1 * ((t2 - t) / (t2 - t0)) + a2 * ((t - t0) / (t2 - t0));

        curve_points.pushBack(b);

        plugin::Vec2 loc_coord = getLocalCoord(b);
        if (loc_coord.x >= 0 && loc_coord.x <= 255)
            draw_curve_points[(int)loc_coord.x] = loc_coord.y;
    }
}

void CurvePolyLine::drawCatmullOf2(plugin::RenderTargetI* perm, plugin::Color color, plugin::Vec2 p1, plugin::Vec2 p2, bool drawing) {
    double t0 = 0;
    double t1 = getCatmullCoeff(t0, p1, p2);

    for (double i = 0; i <= 1; i += 0.001) {
        double t = lerp(t0, t1, i);
        plugin::Vec2 drawPnt = p1 * ((t1 - t) / (t1 - t0)) + p2 * ((t - t0) / (t1 - t0));

        curve_points.pushBack(drawPnt);

        plugin::Vec2 loc_coord = getLocalCoord(drawPnt);
        if (loc_coord.x >= 0 && loc_coord.x <= 255)
            draw_curve_points[(int)loc_coord.x] = loc_coord.y;
    }
}

void CurvePolyLine::drawCatmull(plugin::RenderTargetI* perm, plugin::Color color, bool drawing) {
    size_t pointCnt = points.getSize();
    curve_points.clear();
    for (int i = 0; i < sizeof(draw_curve_points) / sizeof(int); i++) draw_curve_points[i] = -1;

    if (pointCnt == 1) {
        curve_points.pushBack(points[0]);

        plugin::Vec2 loc_coord = getLocalCoord(points[0]);
        if (loc_coord.x >= 0 && loc_coord.x <= 255)
            draw_curve_points[(int)loc_coord.x] = loc_coord.y;
        return;
    }
    if (pointCnt == 2) {
        drawCatmullOf2(perm, color, points[0], points[1], drawing);
        return;
    }
    if (pointCnt == 3) {
        drawCatmullOf3(perm, color, points[0], points[1], points[2], drawing);
        drawCatmullOf3(perm, color, points[2], points[1], points[0], drawing);
        return;
    }

    for (size_t i = 0; i < pointCnt - 3; i++) {
        drawCatmullOf4(perm, color, points[i], points[i + 1], points[i + 2], points[i + 3], drawing);
    }
    drawCatmullOf3(perm, color, points[2], points[1], points[0], drawing);
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
    plugin::Vec2 texture_size = {pl_texture->width, pl_texture->height};

    for (uint64_t i = 0; i < texture_size.x; i++) {
        for (uint64_t j = 0; j < texture_size.y; j++) {
            plugin::Color new_pixel = start_texture->pixels[i * (int)texture_size.y + j];
            
            switch (change_col) {
            case CurveWindow::ACTIVE_SUB_WIN::RED_WIN:
                if (new_pixel.r == old_col) {
                    new_pixel.r = new_col;
                    pl_texture->pixels[i * (int)texture_size.y + j] = new_pixel;
                }
                break;
            case CurveWindow::ACTIVE_SUB_WIN::GREEN_WIN:
                if (new_pixel.g == old_col) {
                    new_pixel.g = new_col;
                    pl_texture->pixels[i * (int)texture_size.y + j] = new_pixel;
                }
                break;
            case CurveWindow::ACTIVE_SUB_WIN::BLUE_WIN:
                if (new_pixel.b == old_col) {
                    new_pixel.b = new_col;
                    pl_texture->pixels[i * (int)texture_size.y + j] = new_pixel;
                }
                break;
            default:
                break;
            }
        }
    }
}

plugin::Vec2 CurvePolyLine::getLocalCoord(plugin::Vec2 global_coord) {
    plugin::Vec2 max_unit = coord_plane->getMaxUnit();
    int x_coord = (global_coord.x - position.x) * max_unit.x / size.x;
    int y_coord = (size.y - (global_coord.y - position.y)) * max_unit.y / size.y;

    return {x_coord, y_coord};
}

plugin::Vec2 CurvePolyLine::getLocalToGlobal(plugin::Vec2 local_coord) {
    plugin::Vec2 max_unit = coord_plane->getMaxUnit();
    double x_coord = position.x + local_coord.x * size.x / max_unit.x;
    double y_coord = position.y + size.y - local_coord.y * size.y / max_unit.y;

    return {x_coord, y_coord};
}

CurvePolyLine::CurvePolyLine(plugin::RenderTargetI* _data, plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, CurveCoordPlane* _coord_plane, CurveWindow::ACTIVE_SUB_WIN _active_tab) :
    DefaultWidget(_app, _pos, _size), data(_data), coord_plane(_coord_plane), active_tab(_active_tab) {
    points.pushBack({position.x, position.y + size.y});
    points.pushBack({position.x + size.x, position.y});

    curve_points.pushBack({position.x, position.y + size.y});
    curve_points.pushBack({position.x + size.x, position.y});

    draw_curve_points[0]   = 0;
    draw_curve_points[255] = 255;

    start_texture = data->getTexture();

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

    for (int i = 0; i < 256; i++) {
        prev_colors[i] = i;
    }
}

bool CurvePolyLine::onMousePress(plugin::MouseContext context) {
    if (!coord_plane->getVisible()) return false;

    if (isInside(context.position)) {
        is_active    = true;
        need_catmull = true;

        for (size_t i = 0; i < points.getSize(); i++) {
            if (areSamePoints(points[i], context.position)) active_point = i;
        }

        if (active_point == -1) {
            active_point = addPoint(context.position);
        }

        if (active_point == 0 || active_point == points.getSize() - 1) hooked_border = true;

        return true;
    }
    return false;
}

bool CurvePolyLine::onMouseRelease(plugin::MouseContext context) {
    if (is_active) {
        plugin::Texture pl_texture;
        pl_texture.height = start_texture->height;
        pl_texture.width  = start_texture->width;
        pl_texture.pixels = new plugin::Color[start_texture->height * start_texture->width];

        for (uint64_t i = 0; i < start_texture->height; i++) {
            for (uint64_t j = 0; j < start_texture->width; j++) {
                plugin::Color new_pixel = start_texture->pixels[i * (int)start_texture->width + j];
                pl_texture.pixels[i * (int)start_texture->width + j] = new_pixel;
            }
        }

        int last_present = 0;
        for (int i = 0; i < sizeof(draw_curve_points) / sizeof(int); i++) {
            if (draw_curve_points[i] == -1) draw_curve_points[i] = last_present;
            else                            last_present         = draw_curve_points[i];

            doApply(&pl_texture, active_tab, i, draw_curve_points[i]);
        }

        data->drawTexture({0, 0}, {(double)pl_texture.width, (double)pl_texture.height}, &pl_texture);

        need_catmull  = false;
        is_active     = false;
        hooked_border = false;
        active_point  = -1;

        return true;
    }

    return false;
}

bool CurvePolyLine::onMouseMove(plugin::MouseContext context) {
    if (!coord_plane->getVisible()) return false;

    if (is_active && active_point >= 0) {
        if (hooked_border) {
            if     (context.position.y > position.y + size.y) points[active_point].y = position.y + size.y;
            else if(context.position.y < position.y)          points[active_point].y = position.y;
            else                                              points[active_point].y = context.position.y;
            return true;
        }

        if (move_dir == MoveDir::NONE) {
            for (int i = 0; i < curve_points.getSize(); i++) {
                if (curve_points[i].x < position.x) {
                    curve_points[i].x = position.x;
                    move_dir   = MoveDir::RIGHT;
                    return false;
                }
                if (position.x + size.x < curve_points[i].x) {
                    curve_points[i].x = position.x + size.x;
                    move_dir   = MoveDir::LEFT;
                    return false;
                }
                if (curve_points[i].y < position.y) {
                    curve_points[i].y = position.y;
                    move_dir   = MoveDir::DOWN;
                    return false;
                }
                if (position.y + size.y < curve_points[i].y) {
                    curve_points[i].y = position.y + size.y;
                    move_dir   = MoveDir::UP;
                    return false;
                }
            }
        } else {
            switch (move_dir)
            {
            case MoveDir::UP: {
                if (!(context.position.y < points[active_point].y)) return false;
                else move_dir = MoveDir::NONE;
                break;
            }
            case MoveDir::DOWN: {
                if (!(context.position.y > points[active_point].y)) return false;
                else move_dir = MoveDir::NONE;
                break;
            }
            case MoveDir::LEFT: {
                if (!(context.position.x < points[active_point].x)) return false;
                else move_dir = MoveDir::NONE;
                break;
            }
            case MoveDir::RIGHT: {
                if (!(context.position.x > points[active_point].x)) return false;
                else move_dir = MoveDir::NONE;
                break;
            }
            default:
                break;
            }
        }

        points[active_point] = context.position;
        active_point = trySwapPoint(active_point);
        return true;
    }
    return false;
}

// void CurvePolyLine::move(plugin::Vec2 shift) {
//     for (size_t i = 0; i < points.getSize(); i++) {
//         points[i].x += shift.x;
//         points[i].y += shift.y;
//     }
//     for (size_t i = 0; i < curve_points.getSize(); i++) {
//         curve_points[i].x += shift.x;
//         curve_points[i].y += shift.y;
//     }

//     DefaultWidget::move(shift);
// }

CurvePolyLine::~CurvePolyLine() {
    delete start_texture;
}

void CurvePolyLine::render(plugin::RenderTargetI* rt) {
    if (!coord_plane->getVisible()) return;

    plugin::Vec2 new_pos = host->getPos();
    if (!(new_pos == position)) {
        for (size_t i = 0; i < points.getSize(); i++) {
            points[i] += (new_pos - position);
        }
        for (size_t i = 0; i < curve_points.getSize(); i++) {
            curve_points[i] += (new_pos - position);
        }
    }

    position = new_pos;

    for (int i = 0; i < points.getSize(); i++) {
        rt->drawRect({points[i].x - 5, points[i].y - 5}, {10, 10}, line_color);
    }

    drawCatmull(rt, line_color);
    for (int i = 0; i < sizeof(draw_curve_points) / sizeof(int); i++) {
        if (draw_curve_points[i] == -1) continue;

        plugin::Vec2 global_coord = getLocalToGlobal({i, draw_curve_points[i]});
        rt->drawEllipse(global_coord, {LINE_DIAM, LINE_DIAM}, line_color);
        
        if (i != sizeof(draw_curve_points) / sizeof(int) - 1) {
            if (draw_curve_points[i + 1] == -1) continue;
            plugin::Vec2 global_coord_2 = getLocalToGlobal({i + 1, draw_curve_points[i + 1]});
            rt->drawLine(global_coord, global_coord_2, line_color);
        }
    }
}

bool CurvePolyLine::isInside(plugin::Vec2 pos) {
    for (size_t i = 0; i < curve_points.getSize() - 1; i++) {
        if (isPointOnLine(curve_points[i], curve_points[i + 1], pos)) return true;
    }
    return false;
    // return DefaultWidget::isInside(pos);
}

CurveCoordPlane::CurveCoordPlane(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, plugin::Vec2 _unit, plugin::Vec2 _max_unit) :
    DefaultWidget(_app, _pos, _size), unit(_unit), max_unit(_max_unit) {

}

void CurveCoordPlane::render(plugin::RenderTargetI* rt) {
    if (!is_visible) return;

    position = host->getPos();
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

void CurveWindow::createDownPanel() {
    
}

void CurveWindow::createTopPanel() {
    // panel with close button
    TopPanel* top_panel = new TopPanel(app, LIGHT_BLUE, position, size.x);
    top_panel->setParent(this);
    registerNewWidget(this, top_panel);

    TextButton* on_close = new TextButton(app, position, {TOP_PANE_SIZE, TOP_PANE_SIZE}, new OnCloseClick(this), "X");
    registerNewWidget(top_panel, on_close);

    // tab section
    plugin::Vec2 tab_size = {size.x / 3, TOP_PANE_SIZE};
    red_tab   = new TextButton(app, {position.x                 , position.y + TOP_PANE_SIZE}, tab_size, nullptr, "Red");
    green_tab = new TextButton(app, {position.x +     tab_size.x, position.y + TOP_PANE_SIZE}, tab_size, nullptr, "Green");
    blue_tab  = new TextButton(app, {position.x + 2 * tab_size.x, position.y + TOP_PANE_SIZE}, tab_size, nullptr, "Blue");

    red_tab->setColor(LIGHT_BLUE);

    registerNewWidget(this, red_tab);
    registerNewWidget(this, green_tab);
    registerNewWidget(this, blue_tab);

    // create coord planes
    plugin::Vec2 plane_size = {std::min(size.x - 4 * TOP_PANE_SIZE, size.y - 4 * TOP_PANE_SIZE), std::min(size.x - 4 * TOP_PANE_SIZE, size.y - 4 * TOP_PANE_SIZE)};
    plugin::Vec2 plane_pos  = {position.x + (size.x - plane_size.x) / 2, position.y + (size.y - plane_size.y) / 2 + LINE_SHIFT};

    CurveCoordPlane* red_plane = new CurveCoordPlane(app, plane_pos, plane_size, {51, 51}, {255, 255});
    CurvePolyLine  * red_line  = new CurvePolyLine  (data, app, plane_pos, plane_size, red_plane, CurveWindow::ACTIVE_SUB_WIN::RED_WIN);
    registerNewWidget(this, red_plane);
    registerNewWidget(red_plane, red_line);

    CurveCoordPlane* green_plane = new CurveCoordPlane(app, plane_pos, plane_size, {51, 51}, {255, 255});
    CurvePolyLine  * green_line  = new CurvePolyLine  (data, app, plane_pos, plane_size, green_plane, CurveWindow::ACTIVE_SUB_WIN::GREEN_WIN);
    registerNewWidget(this, green_plane);
    registerNewWidget(green_plane, green_line);
    green_plane->setVisible(false);

    CurveCoordPlane* blue_plane = new CurveCoordPlane(app, plane_pos, plane_size, {51, 51}, {255, 255});
    CurvePolyLine  * blue_line  = new CurvePolyLine  (data, app, plane_pos, plane_size, blue_plane, CurveWindow::ACTIVE_SUB_WIN::BLUE_WIN);
    registerNewWidget(this, blue_plane);
    registerNewWidget(blue_plane, blue_line);
    blue_plane->setVisible(false);

    red_tab  ->setOnClick(new OnTabChangeClick(red_plane, red_tab, green_plane, green_tab, blue_plane, blue_tab, CurveWindow::ACTIVE_SUB_WIN::RED_WIN));
    green_tab->setOnClick(new OnTabChangeClick(red_plane, red_tab, green_plane, green_tab, blue_plane, blue_tab, CurveWindow::ACTIVE_SUB_WIN::GREEN_WIN));
    blue_tab ->setOnClick(new OnTabChangeClick(red_plane, red_tab, green_plane, green_tab, blue_plane, blue_tab, CurveWindow::ACTIVE_SUB_WIN::BLUE_WIN));
}

CurveWindow::CurveWindow(plugin::RenderTargetI* _data, plugin::App* _app, const char* _window_name) : DefaultWidget(_app), app(_app), data(_data) {
    window_name = strdup(_window_name);

    plugin::Vec2 root_size = app->root->getRoot()->getSize();

    size     = {root_size.x / 2, root_size.y / 2};
    position = {root_size.x / 4, root_size.y / 4};
}

bool CurveWindow::onMousePress(plugin::MouseContext context) {
    return isInside(context.position);
}

void CurveWindow::render(plugin::RenderTargetI* rt) {
    position = host->getPos();
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
    CurveWindow* _win = new CurveWindow(data, app, "Curves");

    app->root->createWidgetI(_win);
    _win->host->setSize(_win->getSize());
    _win->host->setPos (_win->getPos());
    app->root->getRoot()->registerSubWidget(_win->host);

    _win->createTopPanel();
}

plugin::Array<const char *> CurveFilter::getParamNames() const {
    plugin::Array<const char*> arr;
    arr.size = 0;

    return arr;
}

plugin::Array<double> CurveFilter::getParams() const {
    plugin::Array<double> arr;
    arr.size = 0;

    return arr;
}

void CurveFilter::setParams(plugin::Array<double> params) {}

plugin::Interface* CurveFilter::getInterface() const {
    return (plugin::Interface*)this;
}

void CurveFilter::selectPlugin() {}