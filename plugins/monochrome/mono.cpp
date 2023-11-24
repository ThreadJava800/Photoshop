#include "mono.h"

plugin::Plugin* getInstance(plugin::App* app) {
    return new MonochromePlugin();
}

MonochromePlugin::MonochromePlugin() {
    id   = 1;
    name = "Monochrome";
    type = plugin::InterfaceType::Filter;

    filter = new MonochromeFilter();
}

plugin::Interface* MonochromePlugin::getInterface() {
    return filter;
}

MonochromePlugin::~MonochromePlugin() {
    delete filter;
}

MonochromeFilter::MonochromeFilter() {
    param_names.size = 1;
    param_names.data = new const char*[param_names.size];

    param_names.data[0] = "Monochrome";
}

void MonochromeFilter::apply(plugin::RenderTargetI *data) {
    if (!data) return;

    plugin::Texture* plTexture = data->getTexture();
    plugin::Vec2 textureSize = {plTexture->width, plTexture->height};
    
    std::cout << textureSize.x << ' ' << textureSize.y << '\n';

    for (int i = 0; i < textureSize.x; i++) {
        for (int j = 0; j < textureSize.y; j++) {
            plugin::Color newPixel = plTexture->pixels[i * (int)textureSize.y + j];

            u_char new_color = (u_char)(newPixel.r * 0.3) + (u_char)(newPixel.g * 0.6) + (u_char)(newPixel.b * 0.1);

            plTexture->pixels[i * (int)textureSize.y + j] = {new_color, new_color, new_color, newPixel.a};
        }
    }

    data->drawTexture({0, 0}, textureSize, plTexture);

    delete[] plTexture->pixels;
    delete   plTexture;
}

plugin::Array<const char *> MonochromeFilter::getParamNames() {
    return param_names;
}

plugin::Array<double> MonochromeFilter::getParams() {
    return {};
}

void MonochromeFilter::setParams(plugin::Array<double> params) {}

MonochromeFilter::~MonochromeFilter() {
    delete[] param_names.data;
}