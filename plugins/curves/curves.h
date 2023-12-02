#ifndef _CURVE_FILTER_h_
#define _CURVE_FILTER_h_

#include <string.h>

#include "../plugin.h"
#include "list.h"

namespace ThreadJava800_Curve_FIlter {

static plugin::Color TRANSPARENT = {0, 0, 0, 0};
static plugin::Color GRAY        = {128, 128, 128, 255};
static plugin::Color BLACK       = {0, 0, 0, 255};
static plugin::Color WHITE       = {255, 255, 255, 255};
static plugin::Color LIGHT_BLUE  = {161, 200, 241, 255};

static const int TOP_PANE_SIZE = 30;
static const int BTN_TXT_PT    = 23;


class DefaultWidget : public plugin::WidgetI {
protected:
    ThreadJava800_List::List<DefaultWidget*>* children   = {};
    plugin::Vec2                              size       = {0, 0};
    plugin::Vec2                              position   = {0, 0};
    plugin::WidgetI*                          parent     = nullptr;
    bool                                      is_alive   = true;
    bool                                      is_visible = true;

public:
    explicit DefaultWidget(plugin::App* _app);
    explicit DefaultWidget(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size);

    bool getVisible();
    void setVisible(bool _vis);

    void             registerSubWidget  (plugin::WidgetI* object) override;
    void             unregisterSubWidget(plugin::WidgetI* object) override;
    plugin::Vec2     getSize            ()                        override;
    void             setSize            (plugin::Vec2)            override;
    plugin::Vec2     getPos             ()                        override;
    void             setPos             (plugin::Vec2)            override;
    bool             isExtern           ()                        override;
    void             setParent          (WidgetI *root)           override;
    plugin::WidgetI* getParent          ()                        override;
    void             move               (plugin::Vec2 shift)      override;
    bool             getAvailable       ()                        override;
    void             setAvailable       (bool)                    override;
    void             render             (plugin::RenderTargetI*)  override;
    void             recalcRegion       ()                        override;

    bool    onMouseMove      (plugin::MouseContext context)    override;
    bool    onMouseRelease   (plugin::MouseContext context)    override;
    bool    onMousePress     (plugin::MouseContext context)    override;
    bool    onKeyboardPress  (plugin::KeyboardContext context) override;
    bool    onKeyboardRelease(plugin::KeyboardContext context) override;
    bool    onClock          (uint64_t delta)                  override;
    uint8_t getPriority      ()                                override;

    virtual bool isInside(plugin::Vec2 pos);

    virtual ~DefaultWidget(); 
};

struct OnClick;
class Button : public DefaultWidget {
private:
    OnClick* on_click = nullptr;

public:
    explicit Button(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, OnClick* _on_click);

    bool onMousePress(plugin::MouseContext context) override;

    ~Button();
};

class TextButton : public Button {
private:
    char* text = nullptr;

public:
    explicit TextButton(plugin::App* _app, plugin::Vec2 pos, plugin::Vec2 size, OnClick* _on_click, const char* _text);

    ~TextButton();

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

class CurveCoordPlane : public DefaultWidget {
private:
    plugin::Vec2 unit     = {0, 0};
    plugin::Vec2 max_unit = {0, 0};

    static constexpr plugin::Vec2 SHIFT_FROM_EDGES = {10, 10};
    static constexpr int          MAX_UNIT_LEN     = 8;

public:
    explicit CurveCoordPlane(plugin::App* _app, plugin::Vec2 _pos, plugin::Vec2 _size, plugin::Vec2 _unit, plugin::Vec2 _max_unit);

    void render(plugin::RenderTargetI*) override;
};

class CurveWindow : public DefaultWidget {
public:
    enum class ACTIVE_SUB_WIN {
        RED_WIN,
        GREEN_WIN,
        BLUE_WIN
    };

private:
    plugin::App*   app         = nullptr;
    ACTIVE_SUB_WIN active_win  = ACTIVE_SUB_WIN::RED_WIN;
    char*          window_name = nullptr;



    void drawFrame(plugin::RenderTargetI*, plugin::Color);
    void createTopPanel();

public:
    explicit CurveWindow(plugin::App* _app, const char* _window_name);

    void render      (plugin::RenderTargetI*) override;
    void setActiveTab(ACTIVE_SUB_WIN _new_win);

    ~CurveWindow();
};

class CurveFilter : public plugin::Plugin, public plugin::FilterI {
private:
    plugin::App* app         = nullptr;
    bool         win_created = false;

public:
    explicit CurveFilter(plugin::App* _app);

    void                        apply        (plugin::RenderTargetI *data)  override;
    plugin::Array<const char *> getParamNames()                             override;
    plugin::Array<double>       getParams    ()                             override;
    void                        setParams    (plugin::Array<double> params) override; 
    plugin::Interface*          getInterface ()                             override;
};

struct OnClick {
    virtual void operator()() = 0;
    virtual ~OnClick()        = default;
};

struct OnCloseClick : OnClick {
private:
    DefaultWidget* widget = nullptr;

public:
    explicit OnCloseClick(DefaultWidget* _widget);
    void operator()() override;
};

struct OnTabChangeClick : OnClick {
private:
    CurveWindow*                curve_win     = nullptr;
    CurveWindow::ACTIVE_SUB_WIN change_to_win = CurveWindow::ACTIVE_SUB_WIN::RED_WIN;

public:
    explicit OnTabChangeClick(CurveWindow* _curve_win, CurveWindow::ACTIVE_SUB_WIN _change_to);
    void operator()() override;
};

};

#endif