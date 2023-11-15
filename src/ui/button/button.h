#ifndef _BUTTON_h_
#define _BUTTON_h_

#include "../../includes.h"
#include "../renderable.h"

class Button : public Widget {
protected:
    MColor color = MColor();

    void*      onClickArgs = nullptr;
    ButtonFunc onClick     = nullptr;
    bool       needFree    = false;

    bool isInside(MPoint checkPoint);

public:
    explicit Button(MPoint _position, MPoint _size, MColor _color, Widget* _parent, ButtonFunc _func = nullptr, void* _args = nullptr, bool _needFree = false);
    virtual ~Button();

    void render      (RenderTarget* renderTarget)   override;
    bool onMousePress(plugin::MouseContext context) override;
};

class TextButton : public Button {
private:
    MFont* font      = nullptr;
    char* text = nullptr;

public:
    explicit TextButton(MPoint _position, MPoint _size, MColor _color, MFont* _font, const char* _text, Widget* _parent, ButtonFunc _func = nullptr, void* _args = nullptr, bool _needFree = false);
    ~TextButton();

    void setText(const char* _text);

    void render(RenderTarget* renderTarget) override;
};

class ImageButton : public Button {
private:
    MImage* image = nullptr;

public:
    explicit ImageButton(MPoint _position, MPoint _size, MImage* _img, Widget* _parent, ButtonFunc _func = nullptr, void* _args = nullptr, bool _needFree = false);
    ~ImageButton();

    void render(RenderTarget* renderTarget) override;
};

#endif