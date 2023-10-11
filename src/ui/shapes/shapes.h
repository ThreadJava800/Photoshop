#ifndef _SHAPES_h_
#define _SHAPES_h_

#include "../../includes.h"
#include "../renderable.h"

class Rectangle : public Widget {
private:
    MColor fillColor = MColor();
    MColor outColor  = MColor();


    
public:
    explicit Rectangle(MPoint _position, MPoint _size, MColor _fillColor, MColor _outColor, Widget* _parent);
    ~Rectangle();

    MPoint getSize();

    void render(RenderTarget* renderTarget) override;
};

#endif