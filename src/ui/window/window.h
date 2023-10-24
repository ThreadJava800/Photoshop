#ifndef _WINDOW_h_
#define _WINDOW_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../menu/menu.h"
#include "../canvas/canvas.h"

class Window : public Widget {
private:
    Menu*        actions = nullptr;
    ToolManager* manager = nullptr; 
    
    void createCanvas    ();
    void createTopPanel  ();
    void createTestWindow();
    
public:
    bool vis = true;

    explicit Window(MPoint _position, MPoint _size, ToolManager *_manager, Widget* _parent, uint8_t _priority = 0);
    explicit Window(MPoint _position, MPoint _size, ToolManager *_manager, Widget* _parent, Menu* _actions, uint8_t _priority = 0);
    ~Window();

    void setActions(Menu* _actions);
    void render(RenderTarget* renderTarget) override;
};

void prioritizeWindow(Window* window);
void onMove(Window* window, MPoint newPos, MPoint oldPos);
void closeFunc(void* window);

#endif