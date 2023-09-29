#ifndef _MENU_h_
#define _MENU_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../button/button.h"

class Menu : public Widget {
private:
    List<Button*>* buttons = nullptr;

public:
    explicit Menu(MPoint _position, List<Button*>* _buttons);
    ~Menu();

    void render(RenderTarget* renderTarget) override;
};

#endif