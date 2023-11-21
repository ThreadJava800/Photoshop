#ifndef _RENDERABLE_h_
#define _RENDERABLE_h_

#include "../includes.h"
#include "../events/events.h"

class Widget;
struct WidgetPtr {
    union {
        Widget*          program_widget;
        plugin::WidgetI* plugin_widget;
    };
    bool          is_extern;

    explicit WidgetPtr();
    explicit WidgetPtr(plugin::WidgetI* _widget);
    explicit WidgetPtr(Widget* _widget);

    plugin::Vec2 getSize();
    plugin::Vec2 getPos ();

    bool onKeyboardPress  (plugin::KeyboardContext key);
    bool onKeyboardRelease(plugin::KeyboardContext key);
    bool onMousePress     (plugin::MouseContext mouse);
    bool onMouseRelease   (plugin::MouseContext mouse);
    bool onMouseMove      (plugin::MouseContext mouse);
    bool onClock          (size_t delta);
    void move             (plugin::Vec2 shift);
    bool isInside         (plugin::Vec2 position);
    void setAvailable     (bool value);
    bool getAvailable     ();

    friend bool operator==(const WidgetPtr& a, const WidgetPtr& b);
};

class Renderable {
public:
    virtual void render(RenderTarget* renderTarget) = 0;
};

class Widget : public Renderable, public plugin::WidgetI {
protected:
    MPoint           position   = MPoint();
    MPoint           size       = MPoint();
    List<WidgetPtr>* subWindows = nullptr;

    RegionSet* regSet = nullptr;

    Widget* parent;
    bool    exists  = true;
    bool    visible = true;

    uint8_t priority = 0;

    void createEmptyRegionSet();

public:
    MColor debColor;

    explicit Widget(MPoint _position, MPoint _size, Widget* _parent, uint8_t _priority = 0);
    explicit Widget(MPoint _position, MPoint _size, Widget* _parent, List<WidgetPtr>* subWindows, uint8_t _priority = 0);
    virtual ~Widget();

    List<WidgetPtr>* getWindows     ();
    bool             getVisible     ();
    RegionSet*       getRegSet      ();

    void setVisible(bool _visible);

    bool onKeyboardPress  (plugin::KeyboardContext context) override;
    bool onKeyboardRelease(plugin::KeyboardContext context) override;
    bool onClock          (uint64_t delta)                  override;
    bool onMousePress     (plugin::MouseContext context)    override;
    bool onMouseRelease   (plugin::MouseContext context)    override;
    bool onMouseMove      (plugin::MouseContext context)    override;

    void             registerSubWidget  (WidgetI* object)            override;
    void             unregisterSubWidget(WidgetI* object)            override;
    plugin::Vec2     getSize            ()                           override;
    void             setSize            (plugin::Vec2 _size)         override;
    plugin::Vec2     getPos             ()                           override;
    void             setPos             (plugin::Vec2 _pos)          override;
    bool             isExtern           ()                           override;
    void             setParent          (WidgetI *root)              override;
    plugin::WidgetI* getParent          ()                           override;
    void             move               (plugin::Vec2 shift)         override;
    bool             getAvailable       ()                           override;
    void             setAvailable       (bool _available)            override;
    void             render             (plugin::RenderTargetI* rt)  override;
    void             recalcRegion       ()                           override;
    uint8_t          getPriority        ()                           override;

    virtual bool       isInside        (MPoint point);
    virtual RegionSet* getDefaultRegSet();

    void render        (RenderTarget* renderTarget) override;

    void registerObject    (Widget* widget);
    void unregisterObject  ();
    void clearRegionSets   ();
    void fillRegionSets    ();
    void fillRegionSetsRoot();
    void prioritizeWindow  ();
};

#endif