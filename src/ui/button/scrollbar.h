#ifndef _SCROLLBAR_h_
#define _SCROLLBAR_h_

#include "button.h"

typedef void (*ScrollFunc)(void* args, MPoint delta);

class ScrollBar : public Button {
protected:
    double     sensitivity = 0;
    MPoint     sliderPos   = MPoint();
    MPoint     sliderSize  = MPoint();

    bool       isMoving   = false;

    void*      scrollArgs  = nullptr;
    ScrollFunc onScroll    = nullptr;
    MPoint     delta       = MPoint();
    MPoint     prevPos     = MPoint();

    virtual bool moveSlider();

public:
    explicit ScrollBar(MPoint _position, MPoint _size, MPoint _sliderPos, MPoint _sliderSize, Widget* _parent, ScrollFunc _func = nullptr, void* _args = nullptr, MPoint _delta = MPoint());

    ScrollFunc getFunc();
    void*      getArgs();

    bool updateSlider(MPoint shift);

    bool onMousePress  (plugin::MouseContext context) override;
    bool onMouseRelease(plugin::MouseContext context) override;
    bool onMouseMove   (plugin::MouseContext context) override;
    void render        (RenderTarget* renderTarget)   override;
    void move          (plugin::Vec2 shift)           override;
};

struct ScrollBtnArgs {
    ScrollBar* scrollBar   = nullptr;
    MPoint     shift       = MPoint();
    MPoint     sliderShift = MPoint();

    explicit ScrollBtnArgs(ScrollBar* _scrollBar, MPoint _shift, MPoint _sliderShift) :
                scrollBar  (_scrollBar),
                shift      (_shift),
                sliderShift(_sliderShift)          {}
};

void onBtnScrollClick(void* args);

class VerticalScrollBar : public ScrollBar {
private:
    void addButtons();

protected:
    bool moveSlider() override;

public:
    explicit VerticalScrollBar(MPoint _position, MPoint _size, MPoint _sliderPos, MPoint _sliderSize, Widget* _parent, ScrollFunc _func = nullptr, void* _args = nullptr, MPoint _delta = MPoint());
};

class HorizontalScrollBar : public ScrollBar {
private:
    void addButtons();

protected:
    bool moveSlider() override;

public:
    explicit HorizontalScrollBar(MPoint _position, MPoint _size, MPoint _sliderPos, MPoint _sliderSize, Widget* _parent, ScrollFunc _func = nullptr, void* _args = nullptr, MPoint _delta = MPoint());
};

#endif