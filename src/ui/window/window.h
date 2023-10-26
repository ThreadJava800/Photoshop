#ifndef _WINDOW_h_
#define _WINDOW_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../menu/menu.h"
#include "../canvas/canvas.h"
#include "../canvas/filters.h"

class Window : public Widget {
private:
    Menu*          actions     = nullptr;
    ToolManager*   manager     = nullptr; 
    FilterManager* filtManager = nullptr;
    
    void createCanvas    ();
    void createTopPanel  ();
    void createTestWindow();
    
public:
    explicit Window(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, uint8_t _priority = 0);
    explicit Window(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, Menu* _actions, uint8_t _priority = 0);
    ~Window();

    bool onMousePressed (MPoint pos, MMouse btn) override;

    void setActions(Menu* _actions);
    void render(RenderTarget* renderTarget) override;
};

class ModalWindow : public Window {
private:
    EventManager* eventMan = nullptr;

    void makeEventPrivate();

public:
    ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent);
    ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, Menu* _actions);
    ~ModalWindow();
};

void prioritizeWindow(Window* window);
void onMove(Window* window, MPoint newPos, MPoint oldPos);
void closeFunc(void* window);

#endif