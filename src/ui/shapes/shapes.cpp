#include "shapes.h"

Rectangle::Rectangle(MPoint _position, MPoint _size, MColor _fillColor, MColor _outColor, Widget* _parent) :
    Widget    (_position, _size, _parent),
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

void Rectangle::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "RenderTarget was null!",);

    renderTarget->drawRect(position, size, fillColor, MColor(sf::Color(TRANSPARENT)), regSet);
    Widget::render(renderTarget);
}