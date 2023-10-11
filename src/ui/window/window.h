#ifndef _WINDOW_h_
#define _WINDOW_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../menu/menu.h"

class Window : public Widget {
private:
    Menu*  actions  = nullptr;
    
    void createTopPanel();
    void createTestWindow();
    
public:
    bool vis = true;

    explicit Window(MPoint _position, MPoint _size, Widget* _parent);
    explicit Window(MPoint _position, MPoint _size, Widget* _parent, Menu* _actions);
    ~Window();

    Widget* getParent();

    void setActions(Menu* _actions);
    void render(RenderTarget* renderTarget) override;
};

void prioritizeWindow(Window* window);
void onMove(Window* window, MPoint newPos, MPoint oldPos);
void closeFunc(void* window);

#endif