#include "regions.h"


MathRectangle::MathRectangle(MPoint _pos, MPoint _size) :
    position(_pos),
    size    (_size)     {}

MathRectangle::~MathRectangle() {
    position = MPoint();
    size     = MPoint();
}

double MathRectangle::top() {
    return position.y;
}

double MathRectangle::l() {
    return position.x;
}

double MathRectangle::r() {
    return position.x + size.x;
}

double MathRectangle::down() {
    return position.y + size.y;
}

MPoint MathRectangle::getPosition() {
    return position;
}

MPoint MathRectangle::getSize() {
    return size;
}

bool MathRectangle::isYInside(double yPoint) {
    return top() <= yPoint && yPoint <= down();
}

bool MathRectangle::isXInside(double xPoint) {
    return l() <= xPoint && xPoint <= r();
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

RegionSet* merge(MathRectangle posOld, MathRectangle posNew) {
    RegionSet* differenceSet = diff(posOld, posNew);
    differenceSet->addRegion(posNew);
    return differenceSet;
}

RegionSet* diff(MathRectangle posOld, MathRectangle posNew) {
    RegionSet* differenceSet = new RegionSet();
    
    if (isIntersected(posOld, posNew)) {

        // posNew is lefter than posOld
        if (posOld.isXInside(posNew.r())) {
            MPoint regPos  = MPoint(posNew.r(), std::max(posOld.top(), posNew.top()));
            MPoint regSize = MPoint(posOld.r() - posNew.r(), 
                                    std::min(posOld.down(), posNew.down()) - std::max(posOld.top(), posNew.top()));

            differenceSet->addRegion(MathRectangle(regPos, regSize));
        }

        // posNew is righter than posOld
        if (posOld.isXInside(posNew.l())) {
            MPoint regPos  = MPoint(posOld.l(), std::max(posOld.top(), posNew.top()));
            MPoint regSize = MPoint(posNew.l() - posOld.l(), 
                                    std::min(posOld.down(), posNew.down()) - std::max(posOld.top(), posNew.top()));

            differenceSet->addRegion(MathRectangle(regPos, regSize));
        }

        // posNew is upper than posOld
        if (posOld.isYInside(posNew.down())) {
            MPoint regPos  = MPoint(posOld.l(), posNew.down());
            MPoint regSize = MPoint(posOld.size.x, posOld.down() - posNew.down());

            differenceSet->addRegion(MathRectangle(regPos, regSize));
        }

        // posNew is lower than posOld
        if (posOld.isYInside(posNew.top())) {
            MPoint regPos  = MPoint(posOld.l(), posOld.top());
            MPoint regSize = MPoint(posOld.size.x, posNew.top() - posOld.top());

            differenceSet->addRegion(MathRectangle(regPos, regSize));
        }
    }

    return differenceSet;
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

void RegionSet::addRegion(MathRectangle region) {
    rectangles->pushBack(region);
}

List<MathRectangle>* RegionSet::getRectangles() {
    return rectangles;
}

void RegionSet::visualize(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Drawable area was null",);
    ON_ERROR(!rectangles, "List was null!",);

    size_t listSize = rectangles->getSize();
    // std::cout << listSize << '\n';
    for (size_t i = 0; i < listSize; i++) {
        MathRectangle rect = (*rectangles)[i];
        renderTarget->drawRect(rect.getPosition(), rect.getSize(), MColor(DEFAULT_COLOR), MColor(DEB_COLS[i % DEB_COLS_CNT]));
    }
}