#include "button.h"

Button::Button(MPoint _position, MPoint _size, MColor _color) :
    Widget(_position),
    size  (_size),
    color (_color)       {}

Button::~Button() {
    size  = MPoint();
    color = MColor();
}

void Button::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(position, size, color);
}

TextButton::TextButton(MPoint _position, MPoint _size, MColor _color, MFont _font, const char* _text) :
    Button(_position, _size, _color),
    font  (_font),
    text  (_text)        {}

TextButton::~TextButton() {
    font = MFont();
    text = nullptr;
}

void TextButton::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    // TODO move to const
    renderTarget->drawText(position, text, color, font, 10);
    renderTarget->drawRect(position, size, color);
}