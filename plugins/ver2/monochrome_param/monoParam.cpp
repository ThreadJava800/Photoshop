#include "monoParam.h"

plugin::Plugin* getInstance(plugin::App* app) {
    return new MonochromeParamPlugin();
}

MonochromeParamPlugin::MonochromeParamPlugin() {
    id   = 2;
    name = "Mono(custom)";
    type = plugin::InterfaceType::Filter;

    filter = new MonochromeParamFilter();
}

plugin::Interface* MonochromeParamPlugin::getInterface() const {
    return filter;
}

void MonochromeParamPlugin::selectPlugin() {}

MonochromeParamFilter::MonochromeParamFilter() {
    const char* color_names[3] = {"RED", "GREEN", "BLUE"};
    param_names = plugin::Array<const char*>(3, color_names);
}

void MonochromeParamFilter::apply(plugin::RenderTargetI *data) {
    if (!data) return;

    plugin::Texture* plTexture = data->getTexture();
    plugin::Vec2 textureSize = {plTexture->width, plTexture->height};
    
    for (int i = 0; i < textureSize.x; i++) {
        for (int j = 0; j < textureSize.y; j++) {
            plugin::Color newPixel = plTexture->pixels[i * (int)textureSize.y + j];

            u_char new_color = (u_char)(newPixel.r * r) + (u_char)(newPixel.g * g) + (u_char)(newPixel.b * b);
            plTexture->pixels[i * (int)textureSize.y + j] = {new_color, new_color, new_color, newPixel.a};
        }
    }

    data->drawTexture({0, 0}, textureSize, plTexture);

    delete plTexture;
}

plugin::Array<const char *> MonochromeParamFilter::getParamNames() const {
    return param_names;
}

plugin::Array<double> MonochromeParamFilter::getParams() const {
    double col_args[3] = {r, g, b};
    return plugin::Array<double>(3, col_args);
}

void MonochromeParamFilter::setParams(plugin::Array<double> params) {
    r = params.data[0] / 255;
    g = params.data[1] / 255;
    b = params.data[2] / 255;
}