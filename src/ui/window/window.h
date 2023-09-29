#ifndef _WINDOW_h_
#define _WINDOW_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../menu/menu.h"

class Window : public Widget {
private:
    MPoint size     = MPoint();
    Menu*  actions  = nullptr;
    
    void drawTopPanel(RenderTarget* renderTarget);
    
public:
    explicit Window(MPoint _position, MPoint _size);
    explicit Window(MPoint _position, MPoint _size, Menu* _actions);
    ~Window();

    void render(RenderTarget* renderTarget) override;
};

#endif