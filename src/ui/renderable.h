#ifndef _RENDERABLE_h_
#define _RENDERABLE_h_

#include "../includes.h"
#include "../../libs/multimedia/multimedia.h"

class Renderable {
public:
    virtual void render(RenderTarget* renderTarget) = 0;
};

class Widget : public Renderable {
private:
    List<Widget*>* subWindows = nullptr;

public:
    virtual bool onKeyPressed (MKeyboard key) = 0;
    virtual bool onKeyReleased(MKeyboard key) = 0;

    virtual bool onMousePressed (MPoint pos, MMouse btn) = 0;
    virtual bool onMouseReleased(MPoint pos, MMouse btn) = 0;
    virtual bool onMouseMove    (MPoint pos, MMouse btn) = 0;

    virtual void registerObject(Widget* widget) = 0;
};

#endif