#ifndef _RENDERABLE_h_
#define _RENDERABLE_h_

#include "../includes.h"
#include "../events/events.h"

class Renderable : public plugin::RenderableI {
public:
    virtual void render(RenderTarget* renderTarget) = 0;
};

class Widget : public Renderable, public plugin::EventProcessableI, public plugin::WidgetI {
protected:
    MPoint         position   = MPoint();
    MPoint         size       = MPoint();
    List<Widget*>* subWindows = nullptr;

    RegionSet* regSet = nullptr;

    Widget* parent  = nullptr;
    bool    exists  = true;
    bool    visible = true;

    uint8_t priority = 0;

    void createEmptyRegionSet();

public:
    MColor debColor;

    explicit Widget(MPoint _position, MPoint _size, Widget* _parent, uint8_t _priority = 0);
    explicit Widget(MPoint _position, MPoint _size, Widget* _parent, List<Widget*>* subWindows, uint8_t _priority = 0);
    virtual ~Widget();

    List<Widget*>* getWindows();
    bool           getVisible();
    RegionSet*     getRegSet ();

    void setVisible  (bool _visible);
    void recalcRegion();

    bool onKeyboardPress  (plugin::KeyboardContext context) override;
    bool onKeyboardRelease(plugin::KeyboardContext context) override;
    bool onClock          (uint64_t delta)                  override;
    bool onMousePress     (plugin::MouseContext context)    override;
    bool onMouseRelease   (plugin::MouseContext context)    override;
    bool onMouseMove      (plugin::MouseContext context)    override;

    void             registerSubWidget  (WidgetI* object)            override;
    void             unregisterSubWidget(WidgetI* object)            override;
    plugin::Vec2     getSize            ()                   const   override;
    void             setSize            (plugin::Vec2 _size)         override;
    plugin::Vec2     getPos             ()                   const   override;
    void             setPos             (plugin::Vec2 _pos)          override;
    void             setParent          (WidgetI *root)              override;
    plugin::WidgetI* getParent          ()                   const   override;
    void             move               (plugin::Vec2 shift)         override;
    bool             getAvailable       ()                   const   override;
    void             setAvailable       (bool _available)            override;
    uint8_t          getPriority        ()                   const   override;

    virtual bool       isInside        (MPoint point);
    virtual RegionSet* getDefaultRegSet();

    void render(RenderTarget* renderTarget) override;
    void render(plugin::RenderTargetI* rt)  override;

    void registerObject    (Widget* widget);
    void unregisterObject  ();
    void clearRegionSets   ();
    void fillRegionSets    ();
    void fillRegionSetsRoot();
    void prioritizeWindow  ();
};

#endif