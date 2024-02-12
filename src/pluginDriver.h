#ifndef _PLUGIN_DRIVER_h_
#define _PLUGIN_DRIVER_h_

#include "plugin.h"
#include "../libs/multimedia/multimedia.h"
#include "../src/ui/window/window.h"

class PluginWidget : public Widget {
private:
    plugin::PluginWidgetI* plugin_widget;

public:
    explicit PluginWidget(Widget* _parent, plugin::PluginWidgetI* _plugin_widget);

    ~PluginWidget();

    bool onKeyboardPress  (plugin::KeyboardContext context) override;
    bool onKeyboardRelease(plugin::KeyboardContext context) override;
    bool onClock          (uint64_t delta)                  override;
    bool onMousePress     (plugin::MouseContext context)    override;
    bool onMouseRelease   (plugin::MouseContext context)    override;
    bool onMouseMove      (plugin::MouseContext context)    override;
    void render           (RenderTarget* rt)                override;
};

class MGUI : public plugin::GuiI {
private:
    plugin::Vec2   size;
    Widget*        root;
    EventManager*  ev_manager;

    List<plugin::Plugin*>* plugin_list;

public:
    explicit MGUI(EventManager* _ev_man, plugin::Vec2, Widget*);

    void addPlugin(plugin::Plugin*);

    virtual plugin::WidgetI* getRoot            () const                        override;
    virtual void             createWidgetI      (plugin::PluginWidgetI* widget) override;
    virtual plugin::Plugin*  queryPlugin        (uint64_t id)                   override;
    virtual plugin::Texture* loadTextureFromFile(const char *filename)          override;

    ~MGUI();
};

#endif