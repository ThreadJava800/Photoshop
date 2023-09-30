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
    text = nullptr;
}

void TextButton::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    // TODO move to const
    renderTarget->drawText(position, text, color, font, 10);
    renderTarget->drawRect(position, size, color);
}

ImageButton::ImageButton(MPoint _position, MPoint _size, MImage _img) :
    Button(_position, _size, MColor(DEFAULT_COLOR)),
    image (_img)   {}

ImageButton::~ImageButton() { }

void ImageButton::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawSprite(position, size, image);
}