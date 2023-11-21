#include "window.h"

bool isCreated = false;

Window::Window(MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, WindowManager* _winManager, bool isCanv, Widget* _parent, uint8_t _priority) :
    Widget     (_position, _size, _parent, _priority),
    manager    (_manager),
    filtManager(_filtManager),
    winManager (_winManager),
    actions    (nullptr),
    windowName (strdup(_windowName))   {
        createTopPanel();
        if (!isCreated) createTestWindow();

        if (isCanv && _winManager) _winManager->getCanvasWindows()->pushBack(this);
        textFont = new MFont(DEFAULT_FONT);
    }

Window::Window(MPoint _position, MPoint _size, const char* _windowName, ToolManager *_manager, FilterManager *_filtManager, WindowManager* _winManager, bool isCanv, Widget* _parent, Menu* _actions, uint8_t _priority) :
    Widget     (_position, _size, _parent, _priority),
    manager    (_manager),
    filtManager(_filtManager),
    winManager (_winManager),
    actions    (_actions),
    windowName (strdup(_windowName))  {
        createTopPanel();
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

void Window::createCanvas() {
    MathRectangle parentRect = MathRectangle(position + MPoint(0, TOP_PANE_SIZE), size - MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE));
    Canvas *canvas = new Canvas(MPoint(position.x, position.y + TOP_PANE_SIZE), MPoint(CANVAS_SIZE, CANVAS_SIZE), manager, filtManager, this, parentRect);
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

void Window::createTopPanel() {
    Menu* topPanel = new Menu(position, MPoint(size.x, TOP_PANE_SIZE), this, this, onMove);
    Rectangle*   topRect = new Rectangle(position, MPoint(size.x, TOP_PANE_SIZE), MColor::LIGHT_BLUE, MColor::BLACK, topPanel);

    MImage* closeImg    = new MImage(CLOSE_BTN);
    MImage* minimizeImg = new MImage(MINIMIZE_BTN);
    MImage* restoreImg  = new MImage(RESTORE_BTN);

    ImageButton* close    = new ImageButton(position,                                         MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), closeImg, topPanel, closeFunc, (void*)this);
    ImageButton* minimize = new ImageButton(position + MPoint(size.x - 2 * TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), minimizeImg, topPanel);
    ImageButton* restore  = new ImageButton(position + MPoint(size.x -     TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), restoreImg, topPanel);

    topPanel->registerObject(topRect);
    topPanel->registerObject(close);
    topPanel->registerObject(minimize);
    topPanel->registerObject(restore);

    registerObject(topPanel);
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

    delete   editBoxes;
    delete[] paramNames.data;
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

            renderTarget->drawText(MPoint(editBox->getPos()) - MPoint(0, 2 * BTN_TXT_PT), paramNames.data[i], MColor::GRAY, textFont, BTN_TXT_PT);
            editBox->render(renderTarget);
        }
    }

    Widget::render(renderTarget);

    renderTarget->drawText(position + MPoint(2 * TOP_PANE_SIZE, 0), windowName, MColor::BLACK, textFont, BTN_TXT_PT, regSet);
}

void onMove(Window* window, MPoint newPos, MPoint oldPos) {
    ON_ERROR(!window, "Window pointer was null!",);

    window->move((newPos - oldPos).toVec2());
    if (window->getParent()) ((Widget*)window->getParent())->fillRegionSets();
}

void closeFunc(void* window) {
    ON_ERROR(!window, "Window pointer was null!",);

    ((Window*)window)->setAvailable(false);
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