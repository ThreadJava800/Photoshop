#ifndef _RECTANGLE_h_
#define _RECTANGLE_h_

#include "../../includes.h"
#include "../renderable.h"

class Rectangle : public Widget {
private:
    MPoint size  = MPoint();
    MColor color = MColor();
    
public:
    explicit Rectangle(MPoint _position, MPoint _size, MColor _color);
    ~Rectangle();

    void render(RenderTarget* renderTarget) override;
};

#endif