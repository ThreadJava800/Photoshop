#include "filters.h"

BrightnessFilter::BrightnessFilter(bool _need_argument) : need_argument(_need_argument) {
    if (need_argument) {
        param_names = plugin::Array<const char*>(1, BRIGHTNESS_PARAM_NAMES);
    } else {
        param_names = plugin::Array<const char*>(0, nullptr);
    }
}

void BrightnessFilter::apply(plugin::RenderTargetI *data) {
    if (!data) return;

    plugin::Texture* plTexture = data->getTexture();
    MPoint textureSize         = MPoint(plTexture->width, plTexture->height);
    MImage pixelData           = MImage(plTexture);

    List<List<MColor>*>* pixelArr = pixelData.getPixels();
    for (int i = 0; i < textureSize.x; i++) {
        for (int j = 0; j < textureSize.y; j++) {
            MColor newPixel = (*(*pixelArr)[i])[j];
            newPixel       += changeValue;

            (*(*pixelArr)[i])[j] =  newPixel;
        }
    }

    pixelData.imgFromPixel(pixelArr);
    plugin::Texture* drawable_texture = pixelData.toPluginTexture();
    data->drawTexture({0, 0}, textureSize.toVec2(), drawable_texture);

    delete drawable_texture;
    delete plTexture;

    for (size_t i = 0; i < pixelArr->getSize(); i++) delete (*pixelArr)[i];
    delete pixelArr;
}

plugin::Array<const char *> BrightnessFilter::getParamNames() const {
    return param_names;
}

plugin::Array<double> BrightnessFilter::getParams() const {
    double values[1] = {changeValue};
    plugin::Array<double> param_arr(1, values);

    return param_arr;
}

void BrightnessFilter::setParams(plugin::Array<double> params) {
    if (need_argument)  changeValue = params.data[0];
}

ColorfulnessFilter::ColorfulnessFilter(double _satur_coeff) : saturCoeff(_satur_coeff) {}

void ColorfulnessFilter::apply(plugin::RenderTargetI *data) {
    if (!data) return;

    plugin::Texture* plTexture = data->getTexture();
    MPoint textureSize         = MPoint(plTexture->width, plTexture->height);
    MImage pixelData           = MImage(plTexture);

    List<List<MColor>*>* pixelArr = pixelData.getPixels();
    for (int i = 0; i < textureSize.x; i++) {
        for (int j = 0; j < textureSize.y; j++) {
            MColor newPixel = (*(*pixelArr)[i])[j];

            if (newPixel == MColor::BLACK || newPixel == MColor::WHITE) continue;

            double p = std::sqrt(newPixel.r * newPixel.r * RCOEFF + 
                                 newPixel.g * newPixel.g * GCOEFF + 
                                 newPixel.b * newPixel.b * BCOEFF);

            int r = std::round(p + (newPixel.r - p) * saturCoeff);
            int g = std::round(p + (newPixel.g - p) * saturCoeff);
            int b = std::round(p + (newPixel.b - p) * saturCoeff);

            if (r > 255 || g > 255 || b > 255 || r < 0 || g < 0 || b < 0) continue;

            newPixel = MColor(r, g, b, 255);

            (*(*pixelArr)[i])[j] = newPixel;
        }
    }

    pixelData.imgFromPixel(pixelArr);
    plugin::Texture* drawable_texture = pixelData.toPluginTexture();
    data->drawTexture({0, 0}, textureSize.toVec2(), drawable_texture);

    delete drawable_texture;
    delete plTexture;

    for (size_t i = 0; i < pixelArr->getSize(); i++) delete (*pixelArr)[i];
    delete pixelArr;
}

plugin::Array<const char *> ColorfulnessFilter::getParamNames() const {
    return param_names;
}

plugin::Array<double> ColorfulnessFilter::getParams() const {
    return plugin::Array<double>(0, nullptr);
}

void ColorfulnessFilter::setParams(plugin::Array<double> params) {}

FilterManager::FilterManager() :
    active    (false),
    lastFilter(nullptr),
    rt        (nullptr)     {}

FilterManager::~FilterManager() {
    if (lastFilter && need_free) delete ((plugin::Plugin*)lastFilter);

    active     = false;
    lastFilter = nullptr;
    rt         = nullptr;
}

void FilterManager::setRenderTarget(plugin::RenderTargetI *target) {
    rt = target;
}

void FilterManager::setFilter(plugin::FilterI *filter) {
    lastFilter = filter;
}

void FilterManager::applyFilter() {
    if (!active || !rt || !lastFilter) return;

    lastFilter->apply(rt);
}

void FilterManager::setNeedFree(bool _need_free) {
    need_free = _need_free;
}

void FilterManager::setActive(bool _active) {
    active = _active;
}

plugin::RenderTargetI* FilterManager::getRT() {
    return rt;
}

plugin::FilterI* FilterManager::getLast() {
    return lastFilter;
}

bool FilterManager::getActive() {
    return active;
}