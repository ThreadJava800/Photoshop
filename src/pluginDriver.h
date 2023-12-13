#ifndef _PLUGIN_DRIVER_h_
#define _PLUGIN_DRIVER_h_

#include "plugin.h"
#include "../libs/multimedia/multimedia.h"
#include "../src/ui/window/window.h"

class MGUI : public plugin::GuiI {
private:
    plugin::Vec2 size;
    Widget*      root;

    EventManager*  ev_manager;

public:
    explicit MGUI(EventManager* _ev_man, plugin::Vec2, Widget*);

    virtual plugin::WidgetI* getRoot            () const                        override;
    virtual void             createWidgetI      (plugin::PluginWidgetI* widget) override;
    virtual plugin::Plugin*  queryPlugin        (uint64_t id)                   override;
    virtual plugin::Texture* loadTextureFromFile(const char *filename)          override;
};

#endif