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

void RegionSet::regionFromObject(RenderTarget* renderTarget, Widget* widget, MPoint size) {
    ON_ERROR(!widget,       "Pointer to widget was null!",);
    ON_ERROR(!renderTarget, "Pointer to drawable was null!",);

    MPoint textStart = renderTarget->getStart();
    MPoint textSize  = renderTarget->getSize ();

    double globalUp    = textStart.y;
    double globalLeft  = textStart.x;
    double globalDown  = textStart.y + textSize.y;
    double globalRight = textStart.x + textSize.x;

    MPoint thisPos = widget->getPosition();

    // create left rectangle
    if (globalLeft < thisPos.x) {
        MathRectangle rect = {.position = MPoint(globalLeft, thisPos.y),
                              .size     = MPoint(thisPos.x - globalLeft, size.y)};
        rectangles->pushBack(rect);
    }
    // create right rectangle
    if (globalRight > thisPos.x + size.x) {
        MathRectangle rect = {.position = MPoint(thisPos.x + size.x, thisPos.y),
                              .size     = MPoint(globalRight - (thisPos.x + size.x), size.y)};
        rectangles->pushBack(rect);
    }
    //create upper rectangle
    if (globalUp < thisPos.y) {
        MathRectangle rect = {.position = MPoint(globalLeft, globalUp),
                              .size     = MPoint(textSize.x, thisPos.y - globalUp)};
        rectangles->pushBack(rect);
    }
    // create down rectangle
    if (globalDown > thisPos.y + size.y) {
        MathRectangle rect = {.position = MPoint(globalLeft, thisPos.y + size.y),
                              .size     = MPoint(textSize.x, globalDown - (thisPos.y + size.y))};
        rectangles->pushBack(rect);
    }
}

void RegionSet::visualize(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Drawable area was null",);

    size_t listSize = rectangles->getSize();
    for (size_t i = 0; i < listSize; i++) {
        MathRectangle rect = (*rectangles)[i];
        renderTarget->drawRect(rect.position, rect.size, rectColor, MColor(YELLOW));
    }
}