#ifndef _MENU_h_
#define _MENU_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../shapes/shapes.h"
#include "../button/button.h"

class Window;
typedef void (*OnMoveFunc)(Window* window, MPoint newPos, MPoint oldPos);
typedef void (*PriorFunc) (Window* window);

class Menu : public Widget {
private:
    MPoint     prevPos   = MPoint();
    bool       isClicked = false;
    Window*    window    = nullptr;

    OnMoveFunc onMove    = nullptr;
    PriorFunc  prior     = nullptr;

public:
    explicit Menu(MPoint _position, MPoint _size, Widget* _parent, Window* _window = nullptr, OnMoveFunc _onMove = nullptr, PriorFunc _prFunc = nullptr);
    ~Menu();

    bool isInside(MPoint checkPoint);

    bool onMousePressed (MPoint pos, MMouse btn) override;
    bool onMouseReleased(MPoint pos, MMouse btn) override;
    bool onMouseMove    (MPoint pos, MMouse btn) override;
};

#endif