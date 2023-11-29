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
    
    void createTopPanel  ();
    void createTestWindow();
    
public:
    explicit Window(MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, WindowManager* _winManager, bool isCanv, Widget* _parent, uint8_t _priority = 0);
    explicit Window(MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, WindowManager* _winManager, bool isCanv, Widget* _parent, Menu* _actions, uint8_t _priority = 0);
    ~Window();

    const char* getName  ();
    void        setName  (const char* _windowName);
    Canvas    * getCanvas();

    void createCanvas(int width = CANVAS_SIZE, int height = CANVAS_SIZE);

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
    explicit ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent);
    explicit ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, Menu* _actions);
    ~ModalWindow();
};

class EditBoxModal : public ModalWindow {
protected:
    List<EditBox*>*            editBoxes     = nullptr;
    ButtonFunc                 onDestroyFunc = nullptr;
    void*                      onDestroyArgs = nullptr;
    plugin::Array<const char*> paramNames;

public:
    explicit EditBoxModal(EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, plugin::Array<const char*> _paramNames);
    explicit EditBoxModal(EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, plugin::Array<const char*> _paramNames, Menu* _actions);

    virtual ~EditBoxModal();

    plugin::Array<double> getParams   ();
    List<EditBox*>*       getEditBoxes();
    void                  setOnDestroy(ButtonFunc _editBoxes);
    void                  setDestrArgs(void* _args);
    void                  addEditBox  (EditBox* _editBox);

    void render (RenderTarget* renderTarget) override;
};

static const MPoint DEFAULT_POS  = MPoint(320, 280);
static const MPoint DEFAULT_SIZE = MPoint(1280, 520);
static const char*  WINDOW_NAME  = "Plugin parameter window";

class PluginParamWindow : public EditBoxModal {
private:
    plugin::Interface* self = nullptr;

public:
    friend void onOkClick(void* args);

    explicit PluginParamWindow(EventManager* _ev_man, Widget* parent, plugin::Array<const char *> param_names, plugin::Interface * _self);

    ~PluginParamWindow();
};

class WindowManager {
private:
    Window*        main_window   = nullptr;
    List<Window*>* canvasWindows = nullptr;

public:
    explicit WindowManager();

    ~WindowManager();

    List<Window*>* getCanvasWindows();
    Window*        getMainWindow();
    void           setMainWindow(Window* _main_window);
};

void onVertScroll(void* args, MPoint delta);
void onMove(Window* window, MPoint newPos, MPoint oldPos);
void closeFunc(void* window);

#endif