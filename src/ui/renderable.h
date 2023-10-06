#ifndef _RENDERABLE_h_
#define _RENDERABLE_h_

#include "../includes.h"
#include "../../libs/multimedia/multimedia.h"

class Renderable {
public:
    virtual void render(RenderTarget* renderTarget) = 0;
};

class Widget;

class RegionSet {
private:
   List<MathRectangle>* rectangles = nullptr;

public:
    explicit RegionSet();
    ~RegionSet();

    size_t getSize();
    List<MathRectangle>* getRectangles();

    void regionFromObject(RenderTarget* renderTarget, Widget* widget, MPoint size);
    void visualize       (RenderTarget* renderTarget);
};

bool          isIntersected  (MathRectangle posOld, MathRectangle posNew);
MathRectangle getIntersection(MathRectangle posOld, MathRectangle posNew);
void          makeRegions    (List<MathRectangle>* set, MathRectangle global, MathRectangle start);
RegionSet*    intersect      (RenderTarget* renderTarget, MathRectangle posOld, MathRectangle posNew);
RegionSet*    merge          (RenderTarget* renderTarget, MathRectangle posOld, MathRectangle posNew);
RegionSet*    diff           (RenderTarget* renderTarget, MathRectangle posOld, MathRectangle posNew);

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

    virtual void              registerObject(Widget* widget) {};
    virtual List<RegionSet*>* getRegionSet  (RenderTarget* renderTarget) {return nullptr;};
};

#endif