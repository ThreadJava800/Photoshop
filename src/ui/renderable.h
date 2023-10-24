#ifndef _RENDERABLE_h_
#define _RENDERABLE_h_

#include "../includes.h"
#include "../events/events.h"

class Renderable {
public:
    virtual void render(RenderTarget* renderTarget) = 0;
};

class Widget : public Renderable, public EventProcessable {
protected:
    MPoint         position   = MPoint();
    MPoint         size       = MPoint();
    List<Widget*>* subWindows = nullptr;

    RegionSet* regSet = nullptr;

    Widget* parent  = nullptr;
    bool    exists  = true;
    bool    visible = true;

    void createEmptyRegionSet();

public:
    MColor debColor;

    explicit Widget(MPoint _position, MPoint _size, Widget* _parent, uint8_t _priority = 0);
    explicit Widget(MPoint _position, MPoint _size, Widget* _parent, List<Widget*>* subWindows, uint8_t _priority = 0);
    virtual ~Widget();

    MPoint         getPosition();
    List<Widget*>* getWindows ();
    void           setExists  (bool val);
    bool           getExists  ();
    RegionSet*     getRegSet  ();
    Widget   *     getParent  ();

    void setParent(Widget* _parent);

    bool onKeyPressed (MKeyboard key) override {return false;};
    bool onKeyReleased(MKeyboard key) override {return false;};

    bool onMousePressed (MPoint pos, MMouse btn) override;
    bool onMouseReleased(MPoint pos, MMouse btn) override;
    bool onMouseMove    (MPoint pos, MMouse btn) override;

    virtual void move          (MPoint shift);
    virtual void registerObject(Widget* widget);
    virtual bool isInside      (MPoint point);

    void render(RenderTarget* renderTarget) override;

    void unregisterObject  ();
    void clearRegionSets   ();
    void fillRegionSets    ();
    void fillRegionSetsRoot();
    void prioritizeWindow  ();
};

#endif