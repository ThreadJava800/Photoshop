#ifndef _MENU_h_
#define _MENU_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../shapes/shapes.h"
#include "../button/button.h"

class Window;
typedef void (*OnMoveFunc)(Window* window, MPoint newPos, MPoint oldPos);

class Menu : public Widget {
private:
    MPoint     prevPos   = MPoint();
    bool       isClicked = false;
    Window*    window    = nullptr;

    OnMoveFunc onMove    = nullptr;

    void calculateRegSet();

public:
    explicit Menu(MPoint _position, MPoint _size, Widget* _parent, Window* _window = nullptr, OnMoveFunc _onMove = nullptr);
    ~Menu();

    bool isInsideBar(MPoint checkPoint);

    bool onMousePress  (plugin::MouseContext context) override;
    bool onMouseRelease(plugin::MouseContext context) override;
    bool onMouseMove   (plugin::MouseContext context) override;

    bool       isInside        (MPoint point)               override;
    void       render          (RenderTarget* renderTarget) override;
    RegionSet* getDefaultRegSet()                           override;
};

#endif