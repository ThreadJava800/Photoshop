#ifndef _PLUGIN_WIDGET_h_
#define _PLUGIN_WIDGET_h_

#include "../../plugin.h"
#include "../../../libs/multimedia/multimedia.h"
#include "../renderable.h"

// plugin::WidgetI::~WidgetI() {};

struct WidgetPtr {
    union {
        Widget*          program_widget;
        plugin::WidgetI* plugin_widget;
    };
    bool          is_extern;
    RenderTarget* program_render_target = nullptr;

    explicit     WidgetPtr(plugin::WidgetI* _widget);
    explicit     WidgetPtr(Widget* _widget, RenderTarget* _program_render_target);

    plugin::Vec2 getSize();
    plugin::Vec2 getPos ();

    bool         onKeyboardPress  (plugin::KeyboardContext key);
    bool         onKeyboardRelease(plugin::KeyboardContext key);
    bool         onMousePress     (plugin::MouseContext mouse);
    bool         onMouseRelease   (plugin::MouseContext mouse);
    bool         onMouseMove      (plugin::MouseContext mouse);
    bool         onClock          (size_t delta);
    void         setAvailable     (bool value);
    bool         getAvailable     ();
    void         render           (plugin::RenderTargetI* rend_target);
};

class PluginWidget : plugin::WidgetI {
private:
    plugin::Vec2 position;
    plugin::Vec2 size;
    uint8_t      priority;

    plugin::WidgetI* parent;
    bool             is_available;
    RegionSet*       reg_set;

    List<WidgetPtr>* children;

    void createDefaultRegSet();

public:

    explicit PluginWidget(uint8_t _priority = 0);
    explicit PluginWidget(plugin::Vec2 _pos, plugin::Vec2 _size, uint8_t _priority = 0);

    //-----------------WIDGET_I OVERRIDE FUNCS------------------
    void registerSubWidget  (plugin::WidgetI* object) override;
    void unregisterSubWidget(plugin::WidgetI* object) override;
    void registerSubWidget  (Widget*          object, RenderTarget* _objRendTarget);
    void unregisterSubWidget(Widget*          object);

    plugin::Vec2 getSize()                   override;
    void         setSize(plugin::Vec2 _size) override;

    plugin::Vec2 getPos()                  override;
    void         setPos(plugin::Vec2 _pos) override;

    bool isExtern() override;

    void             setParent(plugin::WidgetI *root) override;
    plugin::WidgetI *getParent()                      override;

    void move(plugin::Vec2 shift) override;

    bool getAvailable()                   override;
    void setAvailable(bool _is_available) override;

    void render      (plugin::RenderTargetI* rend_target) override;
    void recalcRegion()                                   override;

    ~PluginWidget();

    //-----------------EVENT_PROCCESSABLE_I OVERRIDE FUNCS------------------
    bool onMouseMove      (plugin::MouseContext context)    override;
    bool onMouseRelease   (plugin::MouseContext context)    override;
    bool onMousePress     (plugin::MouseContext context)    override;
    bool onKeyboardPress  (plugin::KeyboardContext context) override;
    bool onKeyboardRelease(plugin::KeyboardContext context) override;

    bool    onClock    (uint64_t delta) override;
    uint8_t getPriority()               override;
};

#endif