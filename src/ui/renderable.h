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

    void* parent   = nullptr;
    bool is_extern = false;

    bool    exists  = true;
    bool    visible = true;

    void createEmptyRegionSet();

public:
    MColor debColor;

    explicit Widget(MPoint _position, MPoint _size, void* _parent, bool _is_extern = false, uint8_t _priority = 0);
    explicit Widget(MPoint _position, MPoint _size, void* _parent, List<Widget*>* subWindows, bool _is_extern = false, uint8_t _priority = 0);
    virtual ~Widget();

    MPoint                 getPosition();
    MPoint                 getSize    ();
    List<Widget*>*         getWindows ();
    void                   setExists  (bool val);
    bool                   getExists  ();
    bool                   getVisible ();
    RegionSet*             getRegSet  ();
    std::pair<void*, bool> getParent  ();

    void setParent (Widget* _parent);
    void setVisible(bool _visible);

    bool onKeyboardPress  (plugin::KeyboardContext context) override;
    bool onKeyboardRelease(plugin::KeyboardContext context) override;

    bool onClock(uint64_t delta) override;

    bool onMousePress   (plugin::MouseContext context) override;
    bool onMouseRelease (plugin::MouseContext context) override;
    bool onMouseMove    (plugin::MouseContext context) override;

    virtual void       move            (MPoint shift);
    virtual void       registerObject  (Widget* widget);
    virtual bool       isInside        (MPoint point);
    virtual RegionSet* getDefaultRegSet();

    void render(RenderTarget* renderTarget) override;

    void unregisterObject  ();
    void clearRegionSets   ();
    void fillRegionSets    ();
    void fillRegionSetsRoot();
    void prioritizeWindow  ();
};

#endif