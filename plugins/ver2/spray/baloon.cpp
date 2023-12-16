#include "baloon.h"

plugin::Plugin* getInstance(plugin::App* app) {
    return new BaloonToolPlugin(app);
}

plugin::Vec2 polarToLin(Polar polar) {
    return {polar.r * cos(polar.phi), polar.r * sin(polar.phi)};
}

Polar linToPolar(plugin::Vec2& linear) {
    return {sqrt(linear.x * linear.x + linear.y * linear.y), atan(linear.y / linear.x)};
}

void BalloonTool::doSpray(plugin::RenderTargetI *data, plugin::Vec2& mouse_pos, plugin::Color color) {
    for (int i = 0; i < ADD_POINT_CNT; i++) {
        int r   = rand() % CIRCLE_RADIUS;
        int phi = rand() % 360;

        plugin::Vec2 lin_point = polarToLin({r, phi});
        data->setPixel({mouse_pos.x + lin_point.x, mouse_pos.y + lin_point.y}, color);
    }
}

BalloonTool::BalloonTool() {
    param_names.size = 0;
    is_active = false;

    srand(time(NULL));
}

const plugin::Texture *BalloonTool::getIcon() const {
    return nullptr;
}

plugin::Array<const char *> BalloonTool::getParamNames() const {
    plugin::Array<const char*> arr;
    arr.size = 0;

    return arr;
}

plugin::Array<double> BalloonTool::getParams() const {
    plugin::Array<double> arr;
    arr.size = 0;

    return arr;
}

void BalloonTool::setParams(plugin::Array<double> params) {}

void BalloonTool::paintOnPress(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    is_active = true;

    last_pos   = context.position;
    last_color = color;
    draw_data  = data;
    doSpray(data, context.position, color);
}

void BalloonTool::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    is_active = false;
}

void BalloonTool::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    if (is_active) {
        last_pos   = context.position;
        last_color = color;
        draw_data  = data;
        doSpray(data, context.position, color);
    }
}

void BalloonTool::disable(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    is_active = false;
}

bool BalloonTool::onMouseMove(plugin::MouseContext context) {
    return false;
}

bool BalloonTool::onMouseRelease(plugin::MouseContext context) {
    return false;
}

bool BalloonTool::onMousePress(plugin::MouseContext context) {
    return false;
}

bool BalloonTool::onKeyboardPress(plugin::KeyboardContext context) {
    return false;
}

bool BalloonTool::onKeyboardRelease(plugin::KeyboardContext context) {
    return false;
}

bool BalloonTool::onClock(uint64_t delta) {
    if (is_active) {
        doSpray(draw_data, last_pos, last_color);
    }

    return false;
}

uint8_t BalloonTool::getPriority() const {
    return 0;
}


BaloonToolPlugin::BaloonToolPlugin(plugin::App* _app) : app(_app) {
    id   = 3;
    name = "Spray";
    type = plugin::InterfaceType::Tool;

    tool = new BalloonTool();

    app->event_manager->registerObject(tool);
}

plugin::Interface* BaloonToolPlugin::getInterface() const {
    return tool;
}