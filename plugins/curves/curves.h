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
    ThreadJava800_List::List<DefaultWidget*>* children = {};
    plugin::Vec2                              size     = {0, 0};
    plugin::Vec2                              position = {0, 0};
    plugin::WidgetI*                          parent   = nullptr;
    bool                                      is_alive = true;

public:
    explicit DefaultWidget(plugin::App* _app);

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
private:
    enum class ACTIVE_SUB_WIN {
        RED_WIN,
        GREEN_WIN,
        BLUE_WIN
    };

    plugin::App*   app        = nullptr;
    ACTIVE_SUB_WIN active_win = ACTIVE_SUB_WIN::RED_WIN;

    int r_diff = 0, g_diff = 0, b_diff = 0;
    char* window_name = nullptr;

    void drawFrame(plugin::RenderTargetI*, plugin::Color);

public:
    explicit CurveWindow(plugin::App* _app, const char* _window_name);

    void render(plugin::RenderTargetI*) override;

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

};

#endif