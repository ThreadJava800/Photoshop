#ifndef _WINDOW_h_
#define _WINDOW_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../menu/menu.h"
#include "../canvas/canvas.h"

class Window : public Widget {
private:
    Menu*  actions  = nullptr;
    
    void createCanvas    ();
    void createTopPanel  ();
    void createTestWindow();
    
public:
    bool vis = true;
    RenderTarget *tempTarget = nullptr;

    explicit Window(MPoint _position, MPoint _size, RenderTarget *_tempTarget, Widget* _parent);
    explicit Window(MPoint _position, MPoint _size, RenderTarget *_tempTarget, Widget* _parent, Menu* _actions);
    ~Window();

    Widget* getParent();

    void setActions(Menu* _actions);
    void render(RenderTarget* renderTarget) override;
};

void prioritizeWindow(Window* window);
void onMove(Window* window, MPoint newPos, MPoint oldPos);
void closeFunc(void* window);

#endif