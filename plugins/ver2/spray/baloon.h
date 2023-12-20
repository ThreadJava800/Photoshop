#ifndef _BALOON_TOOL_h_
#define _BALOON_TOOL_h_

#include <cmath>
#include <ctime>
#include <iostream>

#include "../plugin.h"

plugin::Texture* icon_texture = nullptr;
void freeTexture() {
    delete icon_texture;
}

struct Polar {
    double r;
    double phi;
};

static plugin::Vec2 polarToLin(Polar         polar);
static Polar        linToPolar(plugin::Vec2& linear);

class BalloonTool : public plugin::ToolI, public plugin::EventProcessableI {
private:
    plugin::RenderTargetI*      draw_data;
    plugin::Vec2                last_pos;
    plugin::Color               last_color;
    plugin::Array<const char *> param_names;
    bool                        is_active;

    static constexpr int CIRCLE_RADIUS = 10;
    static constexpr int ADD_POINT_CNT = 10;

    void doSpray(plugin::RenderTargetI *data, plugin::Vec2& mouse_pos, plugin::Color color);

public:
    explicit BalloonTool();

    const plugin::Texture *getIcon() const override;

    plugin::Array<const char *> getParamNames()                             const override;
    plugin::Array<double>       getParams    ()                             const override;
    void                        setParams    (plugin::Array<double> params)       override; 

    void paintOnPress  (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void disable       (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;

    bool    onMouseMove      (plugin::MouseContext context)          override;
    bool    onMouseRelease   (plugin::MouseContext context)          override;
    bool    onMousePress     (plugin::MouseContext context)          override;
    bool    onKeyboardPress  (plugin::KeyboardContext context)       override;
    bool    onKeyboardRelease(plugin::KeyboardContext context)       override;
    bool    onClock          (uint64_t delta)                        override;
    uint8_t getPriority      ()                                const override;
};

class BaloonToolPlugin : public plugin::Plugin {
private:
    plugin::App* app;
    BalloonTool* tool;

public:
    explicit BaloonToolPlugin(plugin::App* _app);

    plugin::Interface *getInterface() const override;
    void               selectPlugin()       override {};
};

#endif