#ifndef _BALOON_TOOL_h_
#define _BALOON_TOOL_h_

#include "../plugin.h"

class BalloonTool : public plugin::ToolI {
private:
    plugin::Array<const char *> param_names;
    bool                        is_active;

public:
    explicit BalloonTool();

    const plugin::Texture *getIcon() override;

    plugin::Array<const char *> getParamNames()                             override;
    plugin::Array<double>       getParams    ()                             override;
    void                        setParams    (plugin::Array<double> params) override; 

    void paintOnPress  (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void disable       (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
};

class BaloonToolPlugin : public plugin::Plugin {
private:
    plugin::App* app;
    BalloonTool* tool;

public:
    explicit BaloonToolPlugin();

    plugin::Interface *getInterface() override;
};

#endif