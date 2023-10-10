#ifndef _BUTTON_h_
#define _BUTTON_h_

#include "../../includes.h"
#include "../renderable.h"

typedef void (*ButtonFunc)(void*);

class Button : public Widget {
protected:
    MColor color = MColor();

    void*      onClickArgs = nullptr;
    ButtonFunc onClick     = nullptr;

    bool isInside(MPoint checkPoint);

public:
    explicit Button(MPoint _position, MPoint _size, MColor _color, ButtonFunc _func = nullptr, void* _args = nullptr);
    ~Button();

    void render(RenderTarget* renderTarget, RegionSet* regions) override;
    bool onMousePressed(MPoint pos, MMouse btn)                 override;
};

class TextButton : public Button {
private:
    MFont* font      = nullptr;
    const char* text = nullptr;

public:
    explicit TextButton(MPoint _position, MPoint _size, MColor _color, MFont* _font, const char* _text, ButtonFunc _func = nullptr, void* _args = nullptr);
    ~TextButton();

    void render(RenderTarget* renderTarget, RegionSet* regions) override;
};

class ImageButton : public Button {
private:
    MImage* image = nullptr;

public:
    explicit ImageButton(MPoint _position, MPoint _size, MImage* _img, ButtonFunc _func = nullptr, void* _args = nullptr);
    ~ImageButton();

    void render(RenderTarget* renderTarget, RegionSet* regions) override;
};

#endif