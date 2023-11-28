#include "baloon.h"

plugin::Plugin* getInstance(plugin::App* app) {
    return new BaloonToolPlugin();
}

BalloonTool::BalloonTool() {
    param_names.size = 0;
    is_active = false;
}

const plugin::Texture *BalloonTool::getIcon() {
    return nullptr;
}

plugin::Array<const char *> BalloonTool::getParamNames() {
    plugin::Array<const char*> arr;
    arr.size = 0;

    return arr;
}

plugin::Array<double> BalloonTool::getParams() {
    plugin::Array<double> arr;
    arr.size = 0;

    return arr;
}

void BalloonTool::setParams(plugin::Array<double> params) {}

void BalloonTool::paintOnPress(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    is_active = true;
    data->drawEllipse(context.position, {10, 10}, color);
}

void BalloonTool::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    is_active = false;
}

void BalloonTool::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    if (is_active) data->drawEllipse(context.position, {10, 10}, color);
}

void BalloonTool::disable(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    is_active = false;
}

BaloonToolPlugin::BaloonToolPlugin() {
    id   = 3;
    name = "Spray";
    type = plugin::InterfaceType::Tool;

    tool = new BalloonTool();
}

plugin::Interface* BaloonToolPlugin::getInterface() {
    return tool;
}