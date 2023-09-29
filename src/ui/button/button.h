#ifndef _BUTTON_h_
#define _BUTTON_h_

#include "../../includes.h"
#include "../renderable.h"

class Button : public Widget {
protected:
    MPoint size  = MPoint();
    MColor color = MColor();

public:
    explicit Button(MPoint _position, MPoint _size, MColor _color);
    ~Button();

    void render(RenderTarget* renderTarget) override;
};

class TextButton : public Button {
private:
    MFont font       = MFont();
    const char* text = nullptr;

public:
    explicit TextButton(MPoint _position, MPoint _size, MColor _color, MFont _font, const char* _text);
    ~TextButton();

    void render(RenderTarget* renderTarget) override;
};

// class ImageButton : public Button {

// };

#endif