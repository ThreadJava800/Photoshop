#ifndef _PLUGIN_DRIVER_h_
#define _PLUGIN_DRIVER_h_

#include "../plugin.h"

class ProgramGUI : public plugin::GuiI {
private:
    plugin::Vec2     size;
    plugin::WidgetI* root;

public:
    explicit ProgramGUI(plugin::Vec2 _size, plugin::WidgetI* _root);

    plugin::Vec2           getSize          ()                                                                  override;
    plugin::RenderTargetI* getRenderTarget  (plugin::Vec2 size, plugin::Vec2 pos, plugin::Plugin *self)         override;
    void                   createParamWindow(plugin::Array<const char *> param_names, plugin::Interface * self) override;
    plugin::WidgetI*       getRoot          ()                                                                  override;
};

#endif