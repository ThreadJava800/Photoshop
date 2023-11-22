#include "filters.h"

BrightnessFilter::BrightnessFilter() {
    param_names.size = 1;
    param_names.data = new const char*[param_names.size];

    param_names.data[0] = "Brightness:";
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
    data->drawTexture({0, 0}, textureSize.toVec2(), pixelData.toPluginTexture());

    for (size_t i = 0; i < pixelArr->getSize(); i++) delete (*pixelArr)[i];
    delete pixelArr;
}

plugin::Array<const char *> BrightnessFilter::getParamNames() {
    return param_names;
}

plugin::Array<double> BrightnessFilter::getParams() {
    plugin::Array<double> param_arr;
    param_arr.size = 1;
    param_arr.data = new double[param_arr.size];

    param_arr.data[0] = changeValue;

    return param_arr;
}

void BrightnessFilter::setParams(plugin::Array<double> params) {
    changeValue = params.data[0];

    delete[] params.data;
}

// MonochromeFilter::MonochromeFilter() {
//     param_names.size = 1;
//     param_names.data = new const char*[param_names.size];

//     param_names.data[0] = "Monochrome";
// }

// void MonochromeFilter::apply(plugin::RenderTargetI *data) {
//     if (!data) return;

//     plugin::Texture* plTexture = data->getTexture();
//     MPoint textureSize         = MPoint(plTexture->width, plTexture->height);
//     MImage pixelData           = MImage(plTexture);

//     List<List<MColor>*>* pixelArr = pixelData.getPixels();
//     for (int i = 0; i < textureSize.x; i++) {
//         for (int j = 0; j < textureSize.y; j++) {
//             MColor newPixel = (*(*pixelArr)[i])[j];

//             int sum = newPixel.r + newPixel.g + newPixel.b;
//             newPixel = MColor(sum / 3, sum / 3, sum / 3, newPixel.a);

//             (*(*pixelArr)[i])[j] =  newPixel;
//         }
//     }

//     pixelData.imgFromPixel(pixelArr);
//     data->drawTexture({0, 0}, textureSize.toVec2(), pixelData.toPluginTexture());

//     for (size_t i = 0; i < pixelArr->getSize(); i++) delete (*pixelArr)[i];
//     delete pixelArr;
// }

// plugin::Array<const char *> MonochromeFilter::getParamNames() {
//     return param_names;
// }

// plugin::Array<double> MonochromeFilter::getParams() {
//     return {};
// }

// void MonochromeFilter::setParams(plugin::Array<double> params) {}

ColorfulnessFilter::ColorfulnessFilter() {
    param_names.size = 1;
    param_names.data = new const char*[param_names.size];

    param_names.data[0] = "Colorfulness";
}

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
    data->drawTexture({0, 0}, textureSize.toVec2(), pixelData.toPluginTexture());

    for (size_t i = 0; i < pixelArr->getSize(); i++) delete (*pixelArr)[i];
    delete pixelArr;
}

plugin::Array<const char *> ColorfulnessFilter::getParamNames() {
    return param_names;
}

plugin::Array<double> ColorfulnessFilter::getParams() {
    return {};
}

void ColorfulnessFilter::setParams(plugin::Array<double> params) {
    if (params.size == 0) return;

    saturCoeff = params.data[0];

    delete[] params.data;
}

FilterManager::FilterManager() :
    active    (false),
    lastFilter(nullptr),
    rt        (nullptr)     {}

FilterManager::~FilterManager() {
    if (lastFilter && need_free) delete lastFilter;

    active     = false;
    lastFilter = nullptr;
    rt         = nullptr;
}

void FilterManager::setRenderTarget(plugin::RenderTargetI *target) {
    rt = target;
}

void FilterManager::setFilter(plugin::FilterI *filter) {
    if (lastFilter && need_free) delete lastFilter;

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