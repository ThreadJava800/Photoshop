#ifndef _PLUGIN_WIDGET_h_
#define _PLUGIN_WIDGET_h_

#include "../../plugin.h"
#include "../../../libs/multimedia/multimedia.h"

plugin::WidgetI::~WidgetI() {};

class PluginWidget : plugin::WidgetI {
private:
    plugin::Vec2 position;
    plugin::Vec2 size;

    plugin::WidgetI* parent;
    bool             is_available;
    RegionSet*       reg_set;

    List<plugin::WidgetI*>* children;

    void createDefaultRegSet();

public:

    explicit PluginWidget();
    explicit PluginWidget(plugin::Vec2 _pos, plugin::Vec2 _size);

    //-----------------WIDGET_I OVERRIDE FUNCS------------------
    void registerSubWidget  (plugin::WidgetI* object) override;
    void unregisterSubWidget(plugin::WidgetI* object) override;

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

    bool onClock(uint64_t delta) override;

};

#endif