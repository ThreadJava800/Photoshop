#ifndef _SCROLLBAR_h_
#define _SCROLLBAR_h_

#include "button.h"

typedef void (*ScrollFunc)(void* args, MPoint delta);

class ScrollBar : public Button {
private:
    double     sensitivity = 0;
    MPoint     sliderPos   = MPoint();
    MPoint     sliderSize  = MPoint();

    bool       isMoving   = false;

    void*      scrollArgs  = nullptr;
    ScrollFunc onScroll    = nullptr;
    MPoint     delta       = MPoint();

public:
    explicit ScrollBar(MPoint _position, MPoint _size, MPoint _sliderPos, MPoint _sliderSize, Widget* _parent, ScrollFunc _func = nullptr, void* _args = nullptr, MPoint _delta = MPoint());

    bool onMousePressed (MPoint pos, MMouse btn)     override;
    bool onMouseMove    (MPoint pos, MMouse btn)     override;
    bool onMouseReleased(MPoint pos, MMouse btn)     override;
    void render         (RenderTarget* renderTarget) override;
    void move           (MPoint shift)               override;
};

#endif