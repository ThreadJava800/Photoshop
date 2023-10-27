#ifndef _WINDOW_h_
#define _WINDOW_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../menu/menu.h"
#include "../canvas/canvas.h"
#include "../canvas/filters.h"
#include "../editbox/editbox.h"

class Window : public Widget {
protected:
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

    FilterManager* getFiltManager();

    void setActions(Menu* _actions);
    void render(RenderTarget* renderTarget) override;
};

class ModalWindow : public Window {
protected:
    EventManager* eventMan = nullptr;
    ButtonFunc  * onClose  = nullptr;
    void        * args     = nullptr;

    void makeEventPrivate();

public:
    explicit ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent);
    explicit ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, Menu* _actions);
    ~ModalWindow();

    void render (RenderTarget* renderTarget) override;
};

class EditBoxModal : public ModalWindow {
private:
    List<EditBox*>* editBoxes     = nullptr;
    ButtonFunc      onDestroyFunc = nullptr;
    void*           onDestroyArgs = nullptr;

public:
    explicit EditBoxModal(EventManager* _eventMan, MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent);
    explicit EditBoxModal(EventManager* _eventMan, MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, Menu* _actions);

    ~EditBoxModal();

    List<EditBox*>* getEditBoxes();
    void            setOnDestroy(ButtonFunc _editBoxes);
    void            setDestrArgs(void* _args);

    void addEditBox(EditBox* _editBox);
};

void onMove(Window* window, MPoint newPos, MPoint oldPos);
void closeFunc(void* window);

#endif