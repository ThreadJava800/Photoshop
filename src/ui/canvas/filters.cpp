#include "filters.h"

BrightnessFilter::BrightnessFilter() {
    List<const char*>* _paramNames = new List<const char*>();
    _paramNames->pushBack("Brightness:");

    paramNames = _paramNames;
}

BrightnessFilter::~BrightnessFilter() {
    delete paramNames;
}

void BrightnessFilter::apply(RenderTarget* rt) {
    if (!rt) return;

    MImage* pixelData = rt->getImage();
    if (!pixelData) return;

    List<List<MColor>*>* pixelArr = pixelData->getPixels();

    MPoint textureSize = rt->getSize();
    for (int i = 0; i < textureSize.x; i++) {
        for (int j = 0; j < textureSize.y; j++) {
            MColor newPixel = (*(*pixelArr)[i])[j];
            newPixel       += changeValue;

            (*(*pixelArr)[i])[j] =  newPixel;
        }
    }

    pixelData->imgFromPixel(pixelArr);
    rt->drawSprite(MPoint(0, 0), textureSize, pixelData);

    for (size_t i = 0; i < pixelArr->getSize(); i++) delete (*pixelArr)[i];
    delete pixelArr;

    delete pixelData;
}

List<double>* BrightnessFilter::getParams()  {
    List<double>* paramList = new List<double>();
    paramList->pushBack(changeValue);

    return paramList;
}

void BrightnessFilter::setParams(List<double>& params) {
    changeValue = params[0];
}

List<const char*>* BrightnessFilter::getParamNames() {
    return paramNames;
}

MonochromeFilter::MonochromeFilter() : Filter() {
    paramNames = new List<const char*>();
    paramNames->pushBack("Monochrome");
}

MonochromeFilter::~MonochromeFilter() {
    delete paramNames;
}

void MonochromeFilter::apply(RenderTarget* rt) {
    if (!rt) return;

    MImage* pixelData = rt->getImage();
    if (!pixelData) return;

    List<List<MColor>*>* pixelArr = pixelData->getPixels();

    MPoint textureSize = rt->getSize();
    for (int i = 0; i < textureSize.x; i++) {
        for (int j = 0; j < textureSize.y; j++) {
            MColor newPixel = (*(*pixelArr)[i])[j];

            int sum = newPixel.r + newPixel.g + newPixel.b;
            newPixel = MColor(sum / 3, sum / 3, sum / 3, newPixel.a);

            (*(*pixelArr)[i])[j] =  newPixel;
        }
    }

    pixelData->imgFromPixel(pixelArr);
    rt->drawSprite(MPoint(0, 0), textureSize, pixelData);

    for (size_t i = 0; i < pixelArr->getSize(); i++) delete (*pixelArr)[i];
    delete pixelArr;

    delete pixelData;
}

List<double>* MonochromeFilter::getParams() {
    return new List<double>();
}

void MonochromeFilter::setParams(List<double>& params) {}

List<const char*>* MonochromeFilter::getParamNames() {
    return paramNames;
}

ColorfulnessFilter::ColorfulnessFilter() : Filter() {
    paramNames = new List<const char*>();
    paramNames->pushBack("Colorfulness");
}

ColorfulnessFilter::~ColorfulnessFilter() {
    delete paramNames;
}

void ColorfulnessFilter::apply(RenderTarget* rt) {
    if (!rt) return;

    MImage* pixelData = rt->getImage();
    if (!pixelData) return;

    List<List<MColor>*>* pixelArr = pixelData->getPixels();

    MPoint textureSize = rt->getSize();
    for (int i = 0; i < textureSize.x; i++) {
        for (int j = 0; j < textureSize.y; j++) {
            MColor newPixel = (*(*pixelArr)[i])[j];

            MColorHSL hslColor = newPixel.toHSL();
            hslColor.s -= SATURATION_SHIFT;
            newPixel = hslColor.toRGB();

            (*(*pixelArr)[i])[j] =  newPixel;
        }
    }

    pixelData->imgFromPixel(pixelArr);
    rt->drawSprite(MPoint(0, 0), textureSize, pixelData);

    for (size_t i = 0; i < pixelArr->getSize(); i++) delete (*pixelArr)[i];
    delete pixelArr;

    delete pixelData;
}

List<double>* ColorfulnessFilter::getParams() {

}

void ColorfulnessFilter::setParams(List<double>& params) {

}

List<const char*>* ColorfulnessFilter::getParamNames() {
    return paramNames;
}

FilterManager::FilterManager() :
    active    (false),
    lastFilter(nullptr),
    rt        (nullptr)     {}

FilterManager::~FilterManager() {
    if (lastFilter) delete lastFilter;

    active     = false;
    lastFilter = nullptr;
    rt         = nullptr;
}

void FilterManager::setRT(RenderTarget* _rt) {
    rt = _rt;
}

void FilterManager::setLast(Filter* _filter) {
    if (lastFilter) delete lastFilter;

    lastFilter = _filter;
}

void FilterManager::setActive(bool _active) {
    active = _active;
}

RenderTarget* FilterManager::getRT() {
    return rt;
}

Filter* FilterManager::getLast() {
    return lastFilter;
}

bool FilterManager::getActive() {
    return active;
}

void FilterManager::applyFilter() {
    if (!active || !rt || !lastFilter) return;

    lastFilter->apply(rt);
}