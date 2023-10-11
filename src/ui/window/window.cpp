#include "window.h"

bool isCreated = false;

Window::Window(MPoint _position, MPoint _size, Widget* _parent) :
    Widget (_position, _size, _parent),
    actions(nullptr)   {
        createTopPanel();
        if (!isCreated) createTestWindow();
    }

Window::Window(MPoint _position, MPoint _size, Widget* _parent, Menu* _actions) :
    Widget (_position, _size, _parent),
    actions(_actions)  {
        createTopPanel();
        registerObject(actions);
        if (!isCreated) createTestWindow();
    }

Window::~Window() {}

Widget* Window::getParent() {
    return parent;
}

void Window::createTopPanel() {
    Menu* topPanel = new Menu(position, MPoint(size.x, TOP_PANE_SIZE), this, this, onMove, prioritizeWindow);
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
    // subWindows->pushBack(topPanel);
}

void Window::createTestWindow() {
    isCreated = true;

    Window* subWin = new Window(position + MPoint(400, 100), MPoint(400, 400), this);
    registerObject(subWin);

    Window* subWin2 = new Window(position + MPoint(600, 200), MPoint(400, 400), this);
    registerObject(subWin2);

    Window* subWin3 = new Window(position + MPoint(300, 300), MPoint(400, 400), this);
    registerObject(subWin3);
}

void Window::setActions(Menu* _actions) {
    actions = _actions;
    registerObject(actions);
}

void Window::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    // if (vis) {
    renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL), MColor(GRAY), regSet);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];
        if (widget && !widget->getExists()) {
            delete widget;
            subWindows->remove(i);
            listSize--;
        }
    }

    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) {
            widget->render(renderTarget);
        }
    }
}
    // }

    // regSet->visualize(renderTarget);

    // RegionSet* inters1 = diff(MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(600, 200),  MPoint(400, 400)), MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(400, 100), MPoint(400, 400)));
    // RegionSet* inters2 = diff(MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(300, 300),  MPoint(400, 400)), MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(400, 100), MPoint(400, 400)));    
    // if (parent) {

    // RegionSet* thisReg = regSet;
    // RegionSet* parrReg = parent->getRegSet();

    // parrReg->subtract(thisReg);

    // if (parrReg) {
    //     parrReg->visualize(renderTarget);
    //     // delete parrReg;
    // }
    // else std::cout << "No inter\n";
    // }


void prioritizeWindow(Window* window) {
    ON_ERROR(!window, "Window pointer was null!",);

    Widget* parent = window->getParent();
    if (parent && parent->getWindows()) parent->getWindows()->swapWithEnd(window);
}

void onMove(Window* window, MPoint newPos, MPoint oldPos) {
    ON_ERROR(!window, "Window pointer was null!",);

    window->move(newPos - oldPos);
}

void closeFunc(void* window) {
    ON_ERROR(!window, "Window pointer was null!",);

    ((Window*)window)->setExists(false);
}