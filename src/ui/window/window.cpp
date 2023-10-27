#include "window.h"

bool isCreated = false;

Window::Window(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, uint8_t _priority) :
    Widget     (_position, _size, _parent, _priority),
    manager    (_manager),
    filtManager(_filtManager),
    actions    (nullptr)   {
        createTopPanel();
        if (!isCreated) createTestWindow();
    }

Window::Window(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, Menu* _actions, uint8_t _priority) :
    Widget     (_position, _size, _parent, _priority),
    manager    (_manager),
    filtManager(_filtManager),
    actions    (_actions)  {
        createTopPanel();
        registerObject(actions);
        if (!isCreated) createTestWindow();
    }

Window::~Window() {}

bool Window::onMousePressed (MPoint pos, MMouse btn) {
    if (isInside(pos)) {
        prioritizeWindow();
    }

    return Widget::onMousePressed(pos, btn);
}

void Window::createCanvas() {
    Canvas *canvas = new Canvas(MPoint(position.x, position.y + TOP_PANE_SIZE), MPoint(size.x, size.y - TOP_PANE_SIZE), manager, filtManager);
    registerObject(canvas);
}

void Window::createTopPanel() {
    Menu* topPanel = new Menu(position, MPoint(size.x, TOP_PANE_SIZE), this, this, onMove);
    Rectangle*   topRect = new Rectangle(position, MPoint(size.x, TOP_PANE_SIZE), MColor(sf::Color(161, 200, 241)), MColor(BLACK), topPanel);

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

    Window* subWin = new Window(position + MPoint(400, 100), MPoint(400, 400), manager, filtManager, this);
    subWin->createCanvas();
    registerObject(subWin);

    Window* subWin2 = new Window(position + MPoint(600, 200), MPoint(400, 400), manager, filtManager, this);
    subWin2->createCanvas();
    registerObject(subWin2);

    Window* subWin3 = new Window(position + MPoint(300, 215), MPoint(300, 600), manager, filtManager, this);
    subWin3->createCanvas();
    registerObject(subWin3);
}

void Window::setActions(Menu* _actions) {
    actions = _actions;
    registerObject(actions);
}

void Window::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL), MColor(TRANSPARENT), regSet);

    Widget::render(renderTarget);
}

void ModalWindow::makeEventPrivate() {
    if (eventMan) {
            eventMan->registerObject(this);

            List<EventType> privateEvents = List<EventType>();
            privateEvents.pushBack(MOUSE_MOVE);
            privateEvents.pushBack(MOUSE_PRESSED);
            privateEvents.pushBack(MOUSE_RELEASED);

            eventMan->privatizeEvents(privateEvents, 1);
    }
}

ModalWindow::ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent) :
    Window  (_position, _size, _manager, _filtManager, _parent, 1),
    eventMan(_eventMan) {
        makeEventPrivate();
    }

ModalWindow::ModalWindow (EventManager* _eventMan, MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, Menu* _actions) :
    Window  (_position, _size, _manager, _filtManager, _parent, _actions, 1),
    eventMan(_eventMan) {
        makeEventPrivate();
    }

ModalWindow::~ModalWindow() {
    eventMan->resetPriorities ();
    eventMan->unregisterObject(this);
}

void ModalWindow::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawFrame(position, size, MColor(GRAY), regSet);

    Widget::render(renderTarget);
}

void onMove(Window* window, MPoint newPos, MPoint oldPos) {
    ON_ERROR(!window, "Window pointer was null!",);

    window->move(newPos - oldPos);
    if (window->getParent()) window->getParent()->fillRegionSets();
}

void closeFunc(void* window) {
    ON_ERROR(!window, "Window pointer was null!",);

    ((Window*)window)->setExists(false);
}