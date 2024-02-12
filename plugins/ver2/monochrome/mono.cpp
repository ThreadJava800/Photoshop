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

plugin::Interface* MonochromePlugin::getInterface() const {
    return filter;
}

void MonochromePlugin::selectPlugin() {

}

MonochromeFilter::MonochromeFilter() {
    param_names = plugin::Array<const char*>();
}

void MonochromeFilter::apply(plugin::RenderTargetI *data) {
    if (!data) return;

    plugin::Texture* plTexture = data->getTexture();
    plugin::Vec2 textureSize = {plTexture->width, plTexture->height};
    
    for (int i = 0; i < textureSize.x; i++) {
        for (int j = 0; j < textureSize.y; j++) {
            plugin::Color newPixel = plTexture->pixels[i * (int)textureSize.y + j];

            u_char new_color = (u_char)(newPixel.r * 0.3) + (u_char)(newPixel.g * 0.6) + (u_char)(newPixel.b * 0.1);

            plTexture->pixels[i * (int)textureSize.y + j] = {new_color, new_color, new_color, newPixel.a};
        }
    }

    data->drawTexture({0, 0}, textureSize, plTexture);

    delete plTexture;
}

plugin::Array<const char *> MonochromeFilter::getParamNames() const {
    return param_names;
}

plugin::Array<double> MonochromeFilter::getParams() const {
    return plugin::Array<double>();
}

void MonochromeFilter::setParams(plugin::Array<double> params) {}