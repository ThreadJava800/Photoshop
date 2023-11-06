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

    void updateSlider(MPoint shift);

    bool onMousePressed (MPoint pos, MMouse btn)     override;
    bool onMouseMove    (MPoint pos, MMouse btn)     override;
    bool onMouseReleased(MPoint pos, MMouse btn)     override;
    void render         (RenderTarget* renderTarget) override;
    void move           (MPoint shift)               override;
};

struct ScrollBtnArgs {
    ScrollBar* scrollBar = nullptr;
    MPoint     shift     = MPoint();

    explicit ScrollBtnArgs(ScrollBar* _scrollBar, MPoint _shift) :
                scrollBar(_scrollBar),
                shift    (_shift)          {}
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