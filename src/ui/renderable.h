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
    MPoint         size       = MPoint();
    List<Widget*>* subWindows = nullptr;

    RegionSet* regSet = nullptr;

    Widget* parent = nullptr;
    bool    exists = true;

    void createEmptyRegionSet();

public:
    explicit Widget(MPoint _position, MPoint _size, Widget* _parent);
    explicit Widget(MPoint _position, MPoint _size, Widget* _parent, List<Widget*>* subWindows);
    virtual ~Widget();

    MPoint         getPosition();
    List<Widget*>* getWindows ();
    void           setExists  (bool val);
    bool           getExists  ();
    RegionSet*     getRegSet  ();
    Widget   *     getParent  ();

    void setParent(Widget* _parent);

    virtual bool onKeyPressed (MKeyboard key) {return false;};
    virtual bool onKeyReleased(MKeyboard key) {return false;};

    virtual bool onMousePressed (MPoint pos, MMouse btn);
    virtual bool onMouseReleased(MPoint pos, MMouse btn);
    virtual bool onMouseMove    (MPoint pos, MMouse btn);

    virtual void move(MPoint shift);

    virtual void registerObject(Widget* widget);

    void clearRegionSets   ();
    void fillRegionSets    ();
    void fillRegionSetsRoot();
};

void updateRegions(Widget* checkWidget, RegionSet* subSet);

#endif