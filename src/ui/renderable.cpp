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

RegionSet::RegionSet() {
    rectangles = new List<MathRectangle>();
}

RegionSet::~RegionSet() {
    delete rectangles;

    rectangles = nullptr;
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
        renderTarget->drawRect(rect.position, rect.size, MColor(DEFAULT_COLOR), MColor(DEB_COLS[i % DEB_COLS_CNT]));
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

bool isIntersected(MathRectangle posOld, MathRectangle posNew) {
    return posOld   .l() < posNew   .r() &&
           posOld   .r() > posNew   .l() &&
           posOld .top() < posNew.down() &&
           posOld.down() > posNew .top();
}

MathRectangle getIntersection(MathRectangle posOld, MathRectangle posNew) {
    if (!isIntersected(posOld, posNew)) {
        return MathRectangle(MPoint(), MPoint());
    }

    MPoint oldLD = MPoint(posOld.l(), posOld.down());
    MPoint oldRH = MPoint(posOld.r(), posOld. top());
    MPoint newLD = MPoint(posNew.l(), posNew.down());
    MPoint newRH = MPoint(posNew.r(), posNew. top());

    MPoint resLD = MPoint(std::max(oldLD.x, newLD.x), std::min(oldLD.y, newLD.y));
    MPoint resRH = MPoint(std::min(oldRH.x, newRH.x), std::max(oldRH.y, newRH.y));
    MPoint resLT = MPoint(resLD.x, resRH.y);
    MPoint resRD = MPoint(resRH.x, resLD.y);

    if (resLD.x > resRH.x || resRH.y > resLD.y) return MathRectangle(MPoint(), MPoint());

    return MathRectangle(resLT, resRD - resLT);
}

RegionSet* intersect(RenderTarget* renderTarget, MathRectangle posOld, MathRectangle posNew) {
    ON_ERROR(!renderTarget, "Drawable area is null!", nullptr);

    if (!isIntersected(posOld, posNew)) {
        return nullptr;
    }

    MathRectangle inter  = getIntersection(posOld, posNew);


    MPoint textStart     = renderTarget->getStart();
    MPoint textSize      = renderTarget->getSize ();
    MathRectangle global = MathRectangle(textStart, textSize);

    RegionSet* regSet = new RegionSet();
    makeRegions(regSet->getRectangles(), global, inter);

    return regSet;
}

RegionSet* merge(RenderTarget* renderTarget, MathRectangle posOld, MathRectangle posNew) {
    ON_ERROR(!renderTarget, "Drawable area is null!", nullptr);

    MPoint textStart     = renderTarget->getStart();
    MPoint textSize      = renderTarget->getSize ();
    MathRectangle global = MathRectangle(textStart, textSize);

    if (!isIntersected(posOld, posNew)) {
        RegionSet* firstRect = new RegionSet(); 
        makeRegions(firstRect->getRectangles(), global, posOld);
        makeRegions(firstRect->getRectangles(), global, posNew);

        return firstRect;
    }

    RegionSet* regSet = new RegionSet();

    MathRectangle inters = getIntersection(posOld, posNew);
    if (inters.l() == posNew.l()) {
        if (posNew.top() < inters.top()) {
            MathRectangle posNewUnique 
                = MathRectangle(MPoint(posNew.l(), posNew.top()), MPoint(posNew.size.x, posNew.size.y - inters.size.y));
            makeRegions(regSet->getRectangles(), global, posNewUnique);

            MathRectangle middle 
                = MathRectangle(MPoint(posOld.l(), posOld.top()), MPoint(posOld.size.x + posNew.size.x - inters.size.x, inters.size.y));
            makeRegions(regSet->getRectangles(), global, middle);

            MathRectangle posOldUnique 
                = MathRectangle(MPoint(posOld.l(), posOld.top() + inters.size.y), MPoint(posOld.size.x, posOld.size.y - inters.size.y));
            makeRegions(regSet->getRectangles(), global, posOldUnique);
        } else {
            MathRectangle posNewUnique 
                = MathRectangle(MPoint(posNew.l(), posNew.top() + inters.size.y), MPoint(posNew.size.x, posNew.size.y - inters.size.y));
            makeRegions(regSet->getRectangles(), global, posNewUnique);

            MathRectangle middle 
                = MathRectangle(MPoint(posOld.l(), posNew.top()), MPoint(posOld.size.x + posNew.size.x - inters.size.x, inters.size.y));
            makeRegions(regSet->getRectangles(), global, middle);

            MathRectangle posOldUnique 
                = MathRectangle(MPoint(posOld.l(), posOld.top()), MPoint(posOld.size.x, posOld.size.y - inters.size.y));
            makeRegions(regSet->getRectangles(), global, posOldUnique);
        }

        return regSet;
    }

    if (inters.l() == posOld.l()) {
        if (posNew.top() >= inters.top()) {
            MathRectangle posNewUnique 
                = MathRectangle(MPoint(posNew.l(), posNew.top() + inters.size.y), MPoint(posNew.size.x, posNew.size.y - inters.size.y));
            makeRegions(regSet->getRectangles(), global, posNewUnique);

            MathRectangle middle 
                = MathRectangle(MPoint(posNew.l(), posNew.top()), MPoint(posNew.size.x + posOld.size.x - inters.size.x, inters.size.y));
            makeRegions(regSet->getRectangles(), global, middle);

            MathRectangle posOldUnique 
                = MathRectangle(MPoint(posOld.l(), posOld.top()), MPoint(posOld.size.x, posOld.size.y - inters.size.y));
            makeRegions(regSet->getRectangles(), global, posOldUnique);
        } else {
            MathRectangle posNewUnique 
                = MathRectangle(MPoint(posNew.l(), posNew.top()), MPoint(posNew.size.x, posNew.size.y - inters.size.y));
            makeRegions(regSet->getRectangles(), global, posNewUnique);

            MathRectangle middle 
                = MathRectangle(MPoint(posNew.l(), posOld.top()), MPoint(posNew.size.x + posOld.size.x - inters.size.x, inters.size.y));
            makeRegions(regSet->getRectangles(), global, middle);

            MathRectangle posOldUnique 
                = MathRectangle(MPoint(posOld.l(), posOld.top() + inters.size.y), MPoint(posOld.size.x, posOld.size.y - inters.size.y));
            makeRegions(regSet->getRectangles(), global, posOldUnique);
        }
    }

    return regSet;
}

RegionSet* diff(RenderTarget* renderTarget, MathRectangle posOld, MathRectangle posNew) {
    ON_ERROR(!renderTarget, "Drawable area is null!", nullptr);

    RegionSet* regSet = new RegionSet();

    return regSet;
}