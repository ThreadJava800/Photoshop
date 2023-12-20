#include "window.h"

bool isCreated = false;

Window::Window(MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, WindowManager* _winManager, bool isCanv, Widget* _parent, uint8_t _priority, bool need_btn) :
    Widget     (_position, _size, _parent, _priority),
    manager    (_manager),
    filtManager(_filtManager),
    winManager (_winManager),
    actions    (nullptr),
    windowName (strdup(_windowName))   {
        createTopPanel(need_btn);
        if (!isCreated) createTestWindow();

        if (isCanv && _winManager) _winManager->getCanvasWindows()->pushBack(this);
        textFont = new MFont(DEFAULT_FONT);
    }

Window::Window(MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, WindowManager* _winManager, bool isCanv, Widget* _parent, Menu* _actions, uint8_t _priority, bool need_btn) :
    Widget     (_position, _size, _parent, _priority),
    manager    (_manager),
    filtManager(_filtManager),
    winManager (_winManager),
    actions    (_actions),
    windowName (strdup(_windowName))  {
        createTopPanel(need_btn);
        registerObject(actions);
        if (!isCreated) createTestWindow();

        if (isCanv && _winManager) _winManager->getCanvasWindows()->pushBack(this);
        textFont = new MFont(DEFAULT_FONT);
    }

Window::~Window() {
    delete textFont;
    free(windowName);

    if (!winManager) return;

    size_t winCnt = winManager->getCanvasWindows()->getSize();
    for (size_t i = 0; i < winCnt; i++) {
        if ((*winManager->getCanvasWindows())[i] == this) winManager->getCanvasWindows()->remove(i);
    }
}

const char* Window::getName() {
    return windowName;
}

void Window::setName(const char* _windowName) {
    free(windowName);
    windowName = strdup(_windowName);
}

Canvas* Window::getCanvas() {
    return canvas;
}

bool Window::onMousePress(plugin::MouseContext context) {
    if (isInside(MPoint(context.position))) {
        prioritizeWindow();
    }

    return Widget::onMousePress(context);
}

FilterManager* Window::getFiltManager() {
    return filtManager;
}

void onVertScroll(void* args, MPoint delta) {
    Canvas* canvas = (Canvas*) args;

    canvas->onScroll(delta * -1);
}

void Window::createCanvas(int width, int height) {
    MathRectangle parentRect = MathRectangle(position + MPoint(0, TOP_PANE_SIZE), size - MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE));
    Canvas *canvas = new Canvas(MPoint(position.x, position.y + TOP_PANE_SIZE), MPoint(width, height), manager, filtManager, this, parentRect);
    registerObject(canvas);

    MathRectangle sliderX = MathRectangle(MPoint(position.x + SCROLLBAR_BTN_H, position.y  + size.y - TOP_PANE_SIZE), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE));
    MathRectangle sliderY = MathRectangle(MPoint(position.x + size.x - TOP_PANE_SIZE, position.y + TOP_PANE_SIZE + SCROLLBAR_BTN_H), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE));

    double deltaY = (canvas->getSize().y - parentRect.getSize().y) / (size.y - sliderY.getSize().y);
    double deltaX = (canvas->getSize().x - parentRect.getSize().x) / (size.x - sliderX.getSize().x);

    VerticalScrollBar* verticalBar = new VerticalScrollBar(MPoint(position.x + size.x - TOP_PANE_SIZE, position.y + TOP_PANE_SIZE), MPoint(TOP_PANE_SIZE, size.y - 2 * TOP_PANE_SIZE), sliderY.getPosition(), sliderY.getSize(), this, onVertScroll, canvas, MPoint(0, deltaY));
    registerObject(verticalBar);

    HorizontalScrollBar* horizontalBar = new HorizontalScrollBar(MPoint(position.x, position.y + size.y - TOP_PANE_SIZE), MPoint(size.x - TOP_PANE_SIZE, TOP_PANE_SIZE), sliderX.getPosition(), sliderX.getSize(), this, onVertScroll, canvas, MPoint(deltaX, 0));
    registerObject(horizontalBar);

    this->canvas = canvas;
}

void Window::createTopPanel(bool need_btn) {
    Menu* topPanel = new Menu(position, MPoint(size.x, TOP_PANE_SIZE), this, this, onMove);
    Rectangle*   topRect = new Rectangle(position, MPoint(size.x, TOP_PANE_SIZE), MColor::LIGHT_BLUE, MColor::BLACK, topPanel);
    topPanel->registerObject(topRect);
    registerObject(topPanel);

    if (!need_btn) return;

    MImage* closeImg    = new MImage(CLOSE_BTN);
    MImage* minimizeImg = new MImage(MINIMIZE_BTN);
    MImage* restoreImg  = new MImage(RESTORE_BTN);

    ImageButton* close    = new ImageButton(position,                                         MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), closeImg, topPanel, closeFunc, (void*)this);
    ImageButton* minimize = new ImageButton(position + MPoint(size.x - 2 * TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), minimizeImg, topPanel);
    ImageButton* restore  = new ImageButton(position + MPoint(size.x -     TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), restoreImg, topPanel);

    topPanel->registerObject(close);
    topPanel->registerObject(minimize);
    topPanel->registerObject(restore);

}

void Window::createTestWindow() {
    isCreated = true;

    Window* subWin2 = new Window(position + MPoint(300, 200), MPoint(400, 400), "Canvas 1", manager, filtManager, winManager, true, this);
    subWin2->createCanvas();
    registerObject(subWin2);

    Window* subWin = new Window(position + MPoint(400, 100), MPoint(1200, 800), "Canvas 2", manager, filtManager, winManager, true, this);
    subWin->createCanvas();
    registerObject(subWin);

    // Window* subWin3 = new Window(position + MPoint(300, 215), MPoint(300, 600), manager, filtManager, this);
    // subWin3->createCanvas();
    // registerObject(subWin3);
}

void Window::setActions(Menu* _actions) {
    actions = _actions;
    registerObject(actions);
}

void Window::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(position, size, DEFAULT_BACK_COL, MColor::TRANSPARENT, regSet);

    Widget::render(renderTarget);

    renderTarget->drawFrame(position, size, MColor::GRAY, regSet);
    renderTarget->drawText(position + MPoint(2 * TOP_PANE_SIZE, 0), windowName, MColor::BLACK, textFont, BTN_TXT_PT, regSet);
}

void ModalWindow::makeEventPrivate() {
    if (eventMan) {
            eventMan->registerObject(this);

            List<EventType> privateEvents = List<EventType>();
            privateEvents.pushBack(plugin::EventType::MouseMove);
            privateEvents.pushBack(plugin::EventType::MousePress);
            privateEvents.pushBack(plugin::EventType::MouseRelease);

            eventMan->privatizeEvents(privateEvents, 1);
    }
}

ModalWindow::ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent) :
    Window  (_position, _size, _windowName, _manager, _filtManager, nullptr, false, _parent, 1),
    eventMan(_eventMan) {
        makeEventPrivate();
    }

ModalWindow::ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, Menu* _actions) :
    Window  (_position, _size, _windowName, _manager, _filtManager, nullptr, false, _parent, _actions, 1),
    eventMan(_eventMan) {
        makeEventPrivate();
    }

ModalWindow::~ModalWindow() {
    eventMan->resetPriorities ();
    eventMan->unregisterObject(this);
}

EditBoxModal::EditBoxModal(EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, plugin::Array<const char*> _paramNames) :
    ModalWindow  (_eventMan, _position, _size, _windowName, _manager, _filtManager, _parent),
    paramNames   (_paramNames) {
        editBoxes = new List<EditBox*>();
    }

EditBoxModal::EditBoxModal(EventManager* _eventMan, MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, plugin::Array<const char*> _paramNames, Menu* _actions) :
    ModalWindow  (_eventMan, _position, _size, _windowName, _manager, _filtManager, _parent, _actions),
    paramNames   (_paramNames) {
        editBoxes = new List<EditBox*>();
    }

EditBoxModal::~EditBoxModal() {
    if (onDestroyFunc) onDestroyFunc(onDestroyArgs);

    delete editBoxes;
}

plugin::Array<double> EditBoxModal::getParams() {
    size_t editBoxesCnt = editBoxes->getSize();
    plugin::Array<double> doubleArgs(editBoxesCnt);

    for (size_t i = 0; i < editBoxesCnt; i++) {
        double doubleArg = atof((*editBoxes)[i]->getText());
        doubleArgs.data[i] = doubleArg;
    }

    return doubleArgs;
}

List<EditBox*>* EditBoxModal::getEditBoxes() {
    return editBoxes;
}

void EditBoxModal::setOnDestroy(ButtonFunc _editBoxes) {
    onDestroyFunc = _editBoxes;
}

void EditBoxModal::setDestrArgs (void* _args) {
    onDestroyArgs = _args;
}

void EditBoxModal::addEditBox(EditBox* _editBox) {
    editBoxes->pushBack(_editBox);
    registerObject     (_editBox);
}

void EditBoxModal::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect (position, size, MColor::WHITE, MColor::TRANSPARENT);
    renderTarget->drawFrame(position, size, MColor::GRAY, regSet);

    size_t nameCnt    = paramNames.size;
    size_t editBoxCnt = editBoxes ->getSize();
    
    if (nameCnt == editBoxCnt) {
        for (size_t i = 0; i < nameCnt; i++) {
            EditBox* editBox = (*editBoxes)[i];

            editBox->render(renderTarget);
        }
    }

    Widget::render(renderTarget);

    renderTarget->drawText(position + MPoint(2 * TOP_PANE_SIZE, 0), windowName, MColor::BLACK, textFont, BTN_TXT_PT, regSet);
}

void onOkClick(void* args) {
    PluginParamWindow* param_win = (PluginParamWindow*)args;

    plugin::Array<double> params = param_win->getParams();
    param_win->self->setParams(params);

    param_win->setAvailable(false);
}

PluginParamWindow::PluginParamWindow(EventManager* _ev_man, Widget* root, plugin::Array<const char *> param_names, plugin::Interface* _self) :
    EditBoxModal(_ev_man, DEFAULT_POS, DEFAULT_SIZE, WINDOW_NAME, nullptr, nullptr, root, param_names),
    self(_self) {

        TextButton* ok_btn = new TextButton(DEFAULT_POS + MPoint(DEFAULT_SIZE.x / 2, DEFAULT_SIZE.y - 50), MPoint(100, 30), DEFAULT_BACK_COL, new MFont(DEFAULT_FONT), "OK", this, onOkClick, this);
        registerObject(ok_btn);

        for (int i = 0; i < param_names.size; i++) {
            EditBox* editBox = new EditBox(DEFAULT_POS + MPoint(100, i * 60 + TOP_PANE_SIZE + 10), MPoint(1080, 50), this, new MFont(DEFAULT_FONT), NUMBERS_ONLY, param_names.data[i]);
            addEditBox(editBox);
        }
}

PluginParamWindow::~PluginParamWindow() {}

void onMove(Window* window, MPoint newPos, MPoint oldPos) {
    ON_ERROR(!window, "Window pointer was null!",);

    window->move((newPos - oldPos).toVec2());
    if (window->getParent()) ((Widget*)window->getParent())->fillRegionSets();
}

void closeFunc(void* window) {
    ON_ERROR(!window, "Window pointer was null!",);

    ((Window*)window)->setAvailable(false);
}

ChooseToolWindow::ChooseToolWindow(MPoint _position, MPoint _size, Widget* _parent, FilterManager* _filt_manager, ToolManager* _tool_manager) :
    Window(_position, _size, "", _tool_manager, _filt_manager, nullptr, false, _parent, 0, false) {

    col_local_y = size.y - 5 * PICKER_BTN_SIZE;

    addColor(MColor::RED);
    addColor(MColor::GREEN);
    addColor(MColor::YELLOW);
    addColor(MColor::BLUE);
    addColor(MColor::BLACK);
    addColor(MColor::CYAN);
    addColor(MColor::MAGENTA);
}

void onToolBtnClick(void* args) {
    ChooseToolArgs* tool_args = (ChooseToolArgs*) args;
    tool_args->filt_manager->setActive(false);
    tool_args->tool_manager->setTool(tool_args->tool);
}

void onColorBntClick(void* args) {
    ChooseColorArgs* col_args = (ChooseColorArgs*) args;
    col_args->tool_manager->setColor(col_args->color.toPlColor());
}

void ChooseToolWindow::addTool(plugin::ToolI* tool) {
    const plugin::Texture* tool_logo = tool->getIcon();

    ChooseToolArgs* tool_arg = new ChooseToolArgs();
    tool_arg->tool = tool;
    tool_arg->filt_manager = filtManager;
    tool_arg->tool_manager = manager;

    if (!tool_logo) {
        registerSubWidget(new ImageButton(position + MPoint(tool_local_x, tool_local_y), MPoint(PICKER_BTN_SIZE, PICKER_BTN_SIZE), new MImage(DEFAULT_LOGO), this, onToolBtnClick, tool_arg, true));
    }
    else {
        registerSubWidget(new ImageButton(position + MPoint(tool_local_x, tool_local_y), MPoint(PICKER_BTN_SIZE, PICKER_BTN_SIZE), new MImage(tool_logo), this, onToolBtnClick, tool_arg, true));
    }

    if (tool_cnt % 2 != 0) {
        tool_local_y += (PICKER_BTN_SIZE + PICKER_EDGE_SHIFT);
        tool_local_x =  PICKER_EDGE_SHIFT;
    }
    else tool_local_x += (PICKER_BTN_SIZE + PICKER_EDGE_SHIFT);
    tool_cnt++;
}

void ChooseToolWindow::addColor(MColor color) {
    ChooseColorArgs* col_args = new ChooseColorArgs();
    col_args->color        = color;
    col_args->tool_manager = manager;

    registerSubWidget(new Button(position + MPoint(col_local_x, col_local_y), MPoint(PICKER_BTN_SIZE, PICKER_BTN_SIZE), color, this, onColorBntClick, col_args, true));

    if (col_cnt % 2 != 0) {
        col_local_y += (PICKER_BTN_SIZE + PICKER_EDGE_SHIFT);
        col_local_x =  PICKER_EDGE_SHIFT;
    }
    else col_local_x += (PICKER_BTN_SIZE + PICKER_EDGE_SHIFT);
    col_cnt++;
}

WindowManager::WindowManager() {
    canvasWindows = new List<Window*>();
}

WindowManager::~WindowManager() {
    delete canvasWindows;
}

List<Window*>* WindowManager::getCanvasWindows() {
    return canvasWindows;
}

Window* WindowManager::getMainWindow() {
    return main_window;
}

void WindowManager::setMainWindow(Window* _main_window) {
    main_window = _main_window;
}