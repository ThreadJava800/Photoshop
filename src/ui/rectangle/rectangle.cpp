#include "rectangle.h"

Rectangle::Rectangle(MPoint _position, MPoint _size, MColor _color) :
    Widget(_position),
    size  (_size),
    color (_color) {}

Rectangle::~Rectangle() {
    delete subWindows;

    subWindows = nullptr;
    position   = MPoint();
    size       = MPoint();
    color      = MColor();
}

void Rectangle::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "RenderTarget was null!",);

    renderTarget->drawRect(position, size, color);
}