#ifndef _CURVE_FILTER_h_
#define _CURVE_FILTER_h_

#include <string.h>
#include <math.h>

#include "../plugin.h"
#include "list.h"

namespace ThreadJava800_Curve_FIlter {

static plugin::Color TRANSPARENT = {0, 0, 0, 0};
static plugin::Color GRAY        = {128, 128, 128, 255};
static plugin::Color BLACK       = {0, 0, 0, 255};
static plugin::Color RED         = {255, 0, 0, 255};
static plugin::Color GREEN       = {0, 255, 0, 255};
static plugin::Color BLUE        = {0, 0, 255, 255};
static plugin::Color WHITE       = {255, 255, 255, 255};
static plugin::Color LIGHT_BLUE  = {161, 200, 241, 255};

static const int    TOP_PANE_SIZE = 30;
static const int    BTN_TXT_PT    = 23;
static const double LINE_SHIFT    = 20;
static const int    LINE_DIAM     = 1;
static const double CATMULL_ALPHA = 0.5;


class DefaultWidget : public plugin::PluginWidgetI {
protected:
    plugin::Vec2                              size       = {0, 0};
    plugin::Vec2                              position   = {0, 0};
    DefaultWidget*                            parent     = nullptr;
    bool                                      is_alive   = true;
    bool                                      is_visible = true;

public:
    plugin::App* app = nullptr;

    explicit DefaultWidget(plugin::App* _app);
    explicit DefaultWidget(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size);

    bool getVisible();
    void setVisible(bool _vis);

    void setParent(DefaultWidget* _parent) {parent = _parent; };

    plugin::Vec2 getSize() {return size;};
    plugin::Vec2 getPos () {return position;};

    void render(plugin::RenderTargetI*) override {};

    bool    onMouseMove      (plugin::MouseContext context)          override {return false;};
    bool    onMouseRelease   (plugin::MouseContext context)          override {return false;};
    bool    onMousePress     (plugin::MouseContext context)          override {return false;};
    bool    onKeyboardPress  (plugin::KeyboardContext context)       override {return false;};
    bool    onKeyboardRelease(plugin::KeyboardContext context)       override {return false;};
    bool    onClock          (uint64_t delta)                        override {return false;};
    uint8_t getPriority      ()                                const override;

    void move(plugin::Vec2 shift);

    virtual bool isInside(plugin::Vec2 pos);

    virtual ~DefaultWidget(); 
};

void registerNewWidget(DefaultWidget* parent, DefaultWidget* child) {
    parent->app->root->createWidgetI(child);
    child->host->setPos (child->getPos());
    child->host->setSize(child->getSize());
    fprintf(stderr, "HOST %p %p\n", parent, parent->host);
    parent->host->registerSubWidget(child->host);
}

struct OnClick;
class Button : public DefaultWidget {
protected:
    OnClick* on_click = nullptr;

public:
    explicit Button(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, OnClick* _on_click);

    bool onMousePress(plugin::MouseContext context) override;

    ~Button();
};

class TextButton : public Button {
private:
    plugin::Color color = WHITE;
    char* text          = nullptr;

public:
    explicit TextButton(plugin::App* _app, plugin::Vec2 pos, plugin::Vec2 size, OnClick* _on_click, const char* _text);

    ~TextButton();

    void setColor  (plugin::Color _color);
    void setOnClick(OnClick* _on_click);

    void render(plugin::RenderTargetI*) override;
};

class TopPanel : public DefaultWidget {
private:
    plugin::Color color      = TRANSPARENT;
    bool          is_clicked = false;
    plugin::Vec2  last_pos   = {0, 0};

public:
    explicit TopPanel(plugin::App* _app, plugin::Color _color, plugin::Vec2 _position, int width);

    void render(plugin::RenderTargetI*) override;
    bool onMousePress  (plugin::MouseContext context) override;
    bool onMouseRelease(plugin::MouseContext context) override;
    bool onMouseMove   (plugin::MouseContext context) override;
};

class CurveWindow : public DefaultWidget {
public:
    enum class ACTIVE_SUB_WIN {
        RED_WIN,
        GREEN_WIN,
        BLUE_WIN
    };

private:
    plugin::App*           app         = nullptr;
    plugin::RenderTargetI* data        = nullptr;
    char*                  window_name = nullptr;

    TextButton* red_tab   = nullptr;
    TextButton* green_tab = nullptr;
    TextButton* blue_tab  = nullptr;

    void drawFrame(plugin::RenderTargetI*, plugin::Color);

public:
    explicit CurveWindow(plugin::RenderTargetI* _data, plugin::App* _app, const char* _window_name);

    void render        (plugin::RenderTargetI*) override;
    void createTopPanel();

    ~CurveWindow();
};

class CurveCoordPlane;
class CurvePolyLine : public DefaultWidget {
private:
    ThreadJava800_List::List<plugin::Vec2> points                 = {};
    ThreadJava800_List::List<plugin::Vec2> curve_points           = {};
    int                                    draw_curve_points[256] = {};
    CurveCoordPlane*                       coord_plane            = nullptr;
    plugin::Color                          line_color             = {};
    CurveWindow::ACTIVE_SUB_WIN            active_tab             = CurveWindow::ACTIVE_SUB_WIN::RED_WIN;
    plugin::RenderTargetI*                 data                   = nullptr;
    plugin::Texture*                       start_texture          = nullptr;
    bool                                   need_catmull           = true;
    bool                                   hooked_border          = false;
    int                                    prev_colors[256]       = {};
    
    bool    is_active    = false;
    int     active_point = -1;

    double getCatmullCoeff(double prevCoeff, plugin::Vec2 p1, plugin::Vec2 p2);
    void   drawCatmullOf4 (plugin::RenderTargetI* perm, plugin::Color color, plugin::Vec2 p0, plugin::Vec2 p1, plugin::Vec2 p2, plugin::Vec2 p3, bool drawing = true);
    void   drawCatmullOf3 (plugin::RenderTargetI* perm, plugin::Color color, plugin::Vec2 p1, plugin::Vec2 p2, plugin::Vec2 p3, bool drawing = true);
    void   drawCatmullOf2 (plugin::RenderTargetI* perm, plugin::Color color, plugin::Vec2 p1, plugin::Vec2 p2, bool drawing = true);
    void   drawCatmull    (plugin::RenderTargetI* perm, plugin::Color color, bool drawing = true);

    size_t       addPoint        (plugin::Vec2 point);
    bool         isPointOnLine   (plugin::Vec2 line_point1, plugin::Vec2 line_point2, plugin::Vec2 check_point);
    bool         areSamePoints   (plugin::Vec2 point1, plugin::Vec2 point2);
    size_t       trySwapPoint    (size_t point);
    void         doApply         (plugin::Texture* pl_texture, CurveWindow::ACTIVE_SUB_WIN change_col, uint8_t old_col, uint8_t new_col);
    plugin::Vec2 getLocalCoord   (plugin::Vec2 global_coord);
    plugin::Vec2 getLocalToGlobal(plugin::Vec2 local_coord);
    // bool         isPointSafe  (plugin::Vec2 point);

    enum class MoveDir {
        DOWN,
        UP,
        LEFT,
        RIGHT,
        NONE
    };

    MoveDir move_dir = MoveDir::NONE;

public:
    explicit CurvePolyLine(plugin::RenderTargetI* _data, plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, CurveCoordPlane* _coord_plane, CurveWindow::ACTIVE_SUB_WIN _active_tab);

    bool onMousePress  (plugin::MouseContext context) override;
    bool onMouseRelease(plugin::MouseContext context) override;
    bool onMouseMove   (plugin::MouseContext context) override;
    void render        (plugin::RenderTargetI*)       override;

    ~CurvePolyLine();

    bool isInside(plugin::Vec2 pos) override;
};

class CurveCoordPlane : public DefaultWidget {
private:
    plugin::Vec2 unit     = {0, 0};
    plugin::Vec2 max_unit = {0, 0};

    static constexpr plugin::Vec2 SHIFT_FROM_EDGES = {10, 10};
    static constexpr int          MAX_UNIT_LEN     = 8;

public:
    explicit CurveCoordPlane(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, plugin::Vec2 _unit, plugin::Vec2 _max_unit);

    void render(plugin::RenderTargetI*) override;

    plugin::Vec2 getMaxUnit();
};

class CurveFilter : public plugin::Plugin, public plugin::FilterI {
private:
    plugin::App* app         = nullptr;

public:
    explicit CurveFilter(plugin::App* _app);

    void                        apply        (plugin::RenderTargetI *data)        override;
    plugin::Array<const char *> getParamNames()                             const override;
    plugin::Array<double>       getParams    ()                             const override;
    void                        setParams    (plugin::Array<double> params)       override; 
    plugin::Interface*          getInterface ()                             const override;
    void                        selectPlugin ()                                   override;
};

struct OnClick {
    virtual void operator()() = 0;
    virtual ~OnClick()        = default;
};

struct OnCloseClick : OnClick {
private:
    DefaultWidget* widget = nullptr;
    plugin::App*   app    = nullptr;

public:
    explicit OnCloseClick(DefaultWidget* _widget);
    void operator()() override;
};

struct OnTabChangeClick : OnClick {
private:
    CurveCoordPlane* red_plane   = nullptr;
    TextButton*      red_tab     = nullptr;
    CurveCoordPlane* green_plane = nullptr;
    TextButton*      green_tab   = nullptr;
    CurveCoordPlane* blue_plane  = nullptr;
    TextButton*      blue_tab    = nullptr;

    CurveWindow::ACTIVE_SUB_WIN this_win = CurveWindow::ACTIVE_SUB_WIN::RED_WIN;

public:
    explicit OnTabChangeClick(CurveCoordPlane* red_plane, TextButton* red_tab, CurveCoordPlane* green_plane, TextButton* green_tab, CurveCoordPlane* blue_plane, TextButton* blue_tab, CurveWindow::ACTIVE_SUB_WIN _this_win);
    void operator()() override;
};

};

#endif