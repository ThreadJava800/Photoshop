#ifndef _RENDERABLE_h_
#define _RENDERABLE_h_

#include "../includes.h"
#include "../../libs/multimedia/multimedia.h"
#include "../controller/regions.h"

class Renderable {
public:
    virtual void render(RenderTarget* renderTarget) = 0;
};

class Widget : public Renderable {
protected:
    MPoint         position   = MPoint();
    List<Widget*>* subWindows = nullptr;

    bool   exists   = true;

public:
    explicit Widget(MPoint _position);
    explicit Widget(MPoint _position, List<Widget*>* subWindows);
    virtual ~Widget();

    MPoint getPosition();
    void   setExists  (bool val);
    bool   getExists  ();

    virtual bool onKeyPressed (MKeyboard key) {return false;};
    virtual bool onKeyReleased(MKeyboard key) {return false;};

    virtual bool onMousePressed (MPoint pos, MMouse btn);
    virtual bool onMouseReleased(MPoint pos, MMouse btn);
    virtual bool onMouseMove    (MPoint pos, MMouse btn);

    virtual void move(MPoint shift);

    virtual void registerObject(Widget* widget) {};
};

#endif