#include "monoParam.h"

plugin::Plugin* getInstance(plugin::App* app) {
    return new MonochromeParamPlugin();
}

MonochromeParamPlugin::MonochromeParamPlugin() {
    id   = 2;
    name = "Monochr.(custom)";
    type = plugin::InterfaceType::Filter;

    filter = new MonochromeParamFilter();
}

plugin::Interface* MonochromeParamPlugin::getInterface() {
    return filter;
}

MonochromeParamFilter::MonochromeParamFilter() {
    param_names.size = 3;
    param_names.data = new const char*[param_names.size];

    param_names.data[0] = "RED";
    param_names.data[1] = "GREEN";
    param_names.data[2] = "BLUE";
}

void MonochromeParamFilter::apply(plugin::RenderTargetI *data) {
    if (!data) return;

    plugin::Texture* plTexture = data->getTexture();
    plugin::Vec2 textureSize = {plTexture->width, plTexture->height};
    
    std::cout << textureSize.x << ' ' << textureSize.y << '\n';

    for (int i = 0; i < textureSize.x; i++) {
        for (int j = 0; j < textureSize.y; j++) {
            plugin::Color newPixel = plTexture->pixels[i * (int)textureSize.y + j];

            u_char new_color = (u_char)(newPixel.r * r) + (u_char)(newPixel.g * g) + (u_char)(newPixel.b * b);

            plTexture->pixels[i * (int)textureSize.y + j] = {new_color, new_color, new_color, newPixel.a};
        }
    }

    data->drawTexture({0, 0}, textureSize, plTexture);

    delete[] plTexture->pixels;
    delete   plTexture;
}

plugin::Array<const char *> MonochromeParamFilter::getParamNames() {
    return param_names;
}

plugin::Array<double> MonochromeParamFilter::getParams() {
    plugin::Array<double> params;
    params.size = 3;
    params.data = new double[params.size];

    params.data[0] = r / 255;
    params.data[1] = g / 255;
    params.data[2] = b / 255;

    return params;
}

void MonochromeParamFilter::setParams(plugin::Array<double> params) {
    r = params.data[0];
    g = params.data[1];
    b = params.data[2];
}