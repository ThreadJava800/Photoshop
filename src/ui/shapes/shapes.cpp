#include "shapes.h"

Rectangle::Rectangle(MPoint _position, MPoint _size, MColor _fillColor, MColor _outColor) :
    Widget    (_position, _size),
    fillColor (_fillColor),
    outColor  (_outColor)    {}

Rectangle::~Rectangle() {
    position   = MPoint();
    size       = MPoint();
    fillColor  = MColor();
    outColor   = MColor();
}

MPoint Rectangle::getSize() {
    return size;
}

void Rectangle::render(RenderTarget* renderTarget, RegionSet* regions) {
    ON_ERROR(!renderTarget, "RenderTarget was null!",);

    renderTarget->_drawRect(position, size, fillColor, outColor);
}