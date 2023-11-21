#ifndef _WINDOW_h_
#define _WINDOW_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../menu/menu.h"
#include "../canvas/canvas.h"
#include "../canvas/filters.h"
#include "../editbox/editbox.h"
#include "../button/scrollbar.h"

class WindowManager;
class Window : public Widget {
protected:
    Menu*          actions     = nullptr;
    ToolManager*   manager     = nullptr; 
    FilterManager* filtManager = nullptr;
    WindowManager* winManager  = nullptr;
    Canvas       * canvas      = nullptr;

    char* windowName = nullptr;
    MFont     * textFont   = nullptr;
    
    void createCanvas    ();
    void createTopPanel  ();
    void createTestWindow();
    
public:
    explicit Window(MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, WindowManager* _winManager, bool isCanv, void* _parent, bool _is_extern = false, uint8_t _priority = 0);
    explicit Window(MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, WindowManager* _winManager, bool isCanv, void* _parent, Menu* _actions, bool _is_extern = false, uint8_t _priority = 0);
    ~Window();

    const char* getName  ();
    void        setName  (const char* _windowName);
    Canvas    * getCanvas();

    bool onMousePress(plugin::MouseContext context) override;

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
    explicit ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, void* _parent, bool _is_extern = false);
    explicit ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, void* _parent, Menu* _actions, bool _is_extern = false);
    ~ModalWindow();
};

class EditBoxModal : public ModalWindow {
private:
    List<EditBox*>*            editBoxes     = nullptr;
    ButtonFunc                 onDestroyFunc = nullptr;
    void*                      onDestroyArgs = nullptr;
    plugin::Array<const char*> paramNames;

public:
    explicit EditBoxModal(EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, void* _parent, plugin::Array<const char*> _paramNames, bool _is_extern = false);
    explicit EditBoxModal(EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, void* _parent, plugin::Array<const char*> _paramNames, Menu* _actions, bool _is_extern = false);

    ~EditBoxModal();

    List<EditBox*>* getEditBoxes();
    void            setOnDestroy(ButtonFunc _editBoxes);
    void            setDestrArgs(void* _args);

    void addEditBox(EditBox* _editBox);

    void render (RenderTarget* renderTarget) override;
};

class WindowManager {
private:
    List<Window*>* canvasWindows = nullptr;

public:
    explicit WindowManager();

    ~WindowManager();

    List<Window*>* getCanvasWindows();
};

void onVertScroll(void* args, MPoint delta);
void onMove(Window* window, MPoint newPos, MPoint oldPos);
void closeFunc(void* window);

#endif