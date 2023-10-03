#ifndef _RENDERABLE_h_
#define _RENDERABLE_h_

#include "../includes.h"
#include "../../libs/multimedia/multimedia.h"

class Renderable {
public:
    virtual void render(RenderTarget* renderTarget) = 0;
};

class Widget : public Renderable {
protected:
    MPoint         position   = MPoint();
    List<Widget*>* subWindows = nullptr;

public:
    explicit Widget(MPoint _position);
    explicit Widget(MPoint _position, List<Widget*>* subWindows);
    virtual ~Widget();

    MPoint getPosition();

    virtual bool onKeyPressed (MKeyboard key) {return false;};
    virtual bool onKeyReleased(MKeyboard key) {return false;};

    virtual bool onMousePressed (MPoint pos, MMouse btn);
    virtual bool onMouseReleased(MPoint pos, MMouse btn) {return false;};
    virtual bool onMouseMove    (MPoint pos, MMouse btn) {return false;};

    virtual void registerObject(Widget* widget) {};
};



#endif