#include "renderable.h"

Widget::Widget(MPoint _position) :
    position(_position) {
        subWindows = new List<Widget*>();
    }

Widget::Widget(MPoint _position, List<Widget*>* _subWindows) :
    position  (_position),
    subWindows(_subWindows)  {}

Widget::~Widget() {
    if (!subWindows) return;

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) delete widget;
    }

    delete subWindows;
    subWindows = nullptr;
}

MPoint Widget::getPosition() {
    return position;
}

bool Widget::onMousePressed(MPoint pos, MMouse btn) {
    ON_ERROR(!subWindows, "List pointer was null!", false);

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) widget->onMousePressed(pos, btn);
    }

    return true;
}

RegionSet::RegionSet(MColor _color) :
    rectColor(_color) {
        rectangles = new List<MathRectangle>();
    }

RegionSet::~RegionSet() {
    delete rectangles;

    rectangles = nullptr;
    rectColor  = MColor();
}

size_t RegionSet::getSize() {
    return rectangles->getSize();
}

List<MathRectangle>* RegionSet::getRectangles() {
    return rectangles;
}

void RegionSet::regionFromObject(RenderTarget* renderTarget, Widget* widget, MPoint size) {
    ON_ERROR(!widget,       "Pointer to widget was null!",);
    ON_ERROR(!renderTarget, "Pointer to drawable was null!",);

    MPoint textStart = renderTarget->getStart();
    MPoint textSize  = renderTarget->getSize ();

    MathRectangle global = MathRectangle(textStart, textSize);
    MathRectangle start  = MathRectangle(widget->getPosition(), size);

    makeRegions(rectangles, global, start);
}

void RegionSet::visualize(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Drawable area was null",);
    ON_ERROR(!rectangles, "List was null!",);

    size_t listSize = rectangles->getSize();
    // std::cout << listSize << '\n';
    for (size_t i = 0; i < listSize; i++) {
        MathRectangle rect = (*rectangles)[i];
        renderTarget->drawRect(rect.position, rect.size, rectColor, MColor(YELLOW));
    }
}

void makeRegions(List<MathRectangle>* set, MathRectangle global, MathRectangle start) {
    ON_ERROR(!set, "Set was null!",);

    double globalUp    = global.top();
    double globalLeft  = global.l();
    double globalDown  = global.down();
    double globalRight = global.r();

    MPoint textSize = global.size;

    MPoint thisPos = start.position;
    MPoint size    = start.size;

    // create left rectangle
    if (globalLeft < thisPos.x) {
        MathRectangle rect = MathRectangle(MPoint(globalLeft, thisPos.y), MPoint(thisPos.x - globalLeft, size.y));
        set->pushBack(rect);
    }
    // create right rectangle
    if (globalRight > thisPos.x + size.x) {
        MathRectangle rect = MathRectangle(MPoint(thisPos.x + size.x, thisPos.y), MPoint(globalRight - (thisPos.x + size.x), size.y));
        set->pushBack(rect);
    }
    //create upper rectangle
    if (globalUp < thisPos.y) {
        MathRectangle rect = MathRectangle(MPoint(globalLeft, globalUp), MPoint(textSize.x, thisPos.y - globalUp));
        set->pushBack(rect);
    }
    // create down rectangle
    if (globalDown > thisPos.y + size.y) {
        MathRectangle rect = MathRectangle(MPoint(globalLeft, thisPos.y + size.y), MPoint(textSize.x, globalDown - (thisPos.y + size.y)));
        set->pushBack(rect);
    }
}

RegionSet* intersect(RenderTarget* renderTarget, MathRectangle posOld, MathRectangle posNew) {
    ON_ERROR(!renderTarget, "Drawable area is null!", nullptr);

    MPoint oldLD = MPoint(posOld.l(), posOld.down());
    MPoint oldRH = MPoint(posOld.r(), posOld. top());
    MPoint newLD = MPoint(posNew.l(), posNew.down());
    MPoint newRH = MPoint(posNew.r(), posNew. top());

    MPoint resLD = MPoint(std::max(oldLD.x, newLD.x), std::min(oldLD.y, newLD.y));
    MPoint resRH = MPoint(std::min(oldRH.x, newRH.x), std::max(oldRH.y, newRH.y));
    MPoint resLT = MPoint(resLD.x, resRH.y);
    MPoint resRD = MPoint(resRH.x, resLD.y);

    if (resLD.x > resRH.x || resRH.y > resLD.y) return nullptr;

    RegionSet*    regSet = new RegionSet(MColor(DEFAULT_COLOR));
    MathRectangle start  = MathRectangle(resLT, resRD - resLT);

    MPoint textStart     = renderTarget->getStart();
    MPoint textSize      = renderTarget->getSize ();
    MathRectangle global = MathRectangle(textStart, textSize);

    makeRegions(regSet->getRectangles(), global, start);

    return regSet;
}

RegionSet* merge(RenderTarget* renderTarget, MathRectangle posOld, MathRectangle posNew) {
    RegionSet* regSet = new RegionSet();



    return regSet;
}

RegionSet* diff(RenderTarget* renderTarget, MathRectangle posOld, MathRectangle posNew) {
    RegionSet* regSet = new RegionSet();

    return regSet;
}