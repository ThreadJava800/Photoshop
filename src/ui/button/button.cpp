#include "button.h"

Button::Button(MPoint _position, MPoint _size, MColor _color, Widget* _parent, ButtonFunc _func, void* _args) :
    Widget     (_position, _size, _parent),
    color      (_color),
    onClick    (_func),
    onClickArgs(_args)       {}

Button::~Button() {
    size  = MPoint();
    color = MColor();
}

void Button::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(position, size, color, MColor(TRANSPARENT));
}

bool Button::onMousePressed(MPoint pos, MMouse btn) {
    if (isInside(pos) && onClick) {
        onClick(onClickArgs);
        return true;
    }
    return false;
}

bool Button::isInside(MPoint checkPoint) {
    return checkPoint.x - position.x >= 0      &&
           checkPoint.x - position.x <= size.x &&
           checkPoint.y - position.y >= 0      &&
           checkPoint.y - position.y <= size.y;
}

TextButton::TextButton(MPoint _position, MPoint _size, MColor _color, MFont* _font, const char* _text, Widget* _parent, ButtonFunc _func, void* _args) :
    Button(_position, _size, _color, _parent, _func, _args),
    font  (_font),
    text  (_text)        {}

TextButton::~TextButton() {
    delete font;

    font = nullptr;
    text = nullptr;
}

void TextButton::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(position, size, color, MColor(TRANSPARENT), regSet);
    renderTarget->drawText(position, text, MColor(BLACK), font, BTN_TXT_PT, regSet);

    Widget::render(renderTarget);
}

ImageButton::ImageButton(MPoint _position, MPoint _size, MImage* _img, Widget* _parent, ButtonFunc _func, void* _args) :
    Button(_position, _size, MColor(DEFAULT_COLOR), _parent, _func, _args),
    image (_img)   {}

ImageButton::~ImageButton() { 
    delete image;

    image = nullptr;
}

void ImageButton::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawSprite(position, size, image, regSet);

    Widget::render(renderTarget);
}