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
            MPoint curPos = MPoint(i, j);

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