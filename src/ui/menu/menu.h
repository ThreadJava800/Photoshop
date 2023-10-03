#ifndef _MENU_h_
#define _MENU_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../shapes/shapes.h"
#include "../button/button.h"

class Menu : public Widget {
public:
    explicit Menu(MPoint _position);
    ~Menu();

    void render(RenderTarget* renderTarget) override;
    void registerObject(Widget* widget)     override;
};

#endif