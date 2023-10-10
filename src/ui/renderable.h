#ifndef _RENDERABLE_h_
#define _RENDERABLE_h_

#include "../includes.h"
#include "../../libs/multimedia/multimedia.h"

class Renderable {
public:
    virtual void render(RenderTarget* renderTarget, RegionSet* regions = nullptr) = 0;
};

class Widget : public Renderable {
protected:
    MPoint         position   = MPoint();
    MPoint         size       = MPoint();
    List<Widget*>* subWindows = nullptr;

    bool   exists   = true;

public:
    explicit Widget(MPoint _position, MPoint _size);
    explicit Widget(MPoint _position, MPoint _size, List<Widget*>* subWindows);
    virtual ~Widget();

    MPoint getPosition();
    void   setExists  (bool val);
    bool   getExists  ();

    List<RegionSet*>* createRegions();

    virtual bool onKeyPressed (MKeyboard key) {return false;};
    virtual bool onKeyReleased(MKeyboard key) {return false;};

    virtual bool onMousePressed (MPoint pos, MMouse btn);
    virtual bool onMouseReleased(MPoint pos, MMouse btn);
    virtual bool onMouseMove    (MPoint pos, MMouse btn);

    virtual void move(MPoint shift);

    virtual void registerObject(Widget* widget) {};
};

#endif