#include "shapes.h"

Rectangle::Rectangle(MPoint _position, MPoint _size, MColor _fillColor, MColor _outColor) :
    Widget    (_position),
    size      (_size),
    fillColor (_fillColor),
    outColor  (_outColor)    {}

Rectangle::~Rectangle() {
    position   = MPoint();
    size       = MPoint();
    fillColor  = MColor();
    outColor   = MColor();
}

void Rectangle::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "RenderTarget was null!",);

    renderTarget->drawRect(position, size, fillColor, outColor);
}