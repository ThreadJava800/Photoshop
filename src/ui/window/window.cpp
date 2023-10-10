#include "window.h"

bool isCreated = false;

Window::Window(MPoint _position, MPoint _size) :
    Widget (_position, _size),
    actions(nullptr)   {
        createTopPanel();
        if (!isCreated) createTestWindow();
    }

Window::Window(MPoint _position, MPoint _size, Menu* _actions) :
    Widget (_position, _size),
    actions(_actions)  {
        createTopPanel();
        subWindows->pushBack(actions);
        if (!isCreated) createTestWindow();
    }

Window::~Window() {}

void Window::createTopPanel() {
    Rectangle*   topRect = new Rectangle(position, MPoint(size.x, TOP_PANE_SIZE), MColor(sf::Color(161, 200, 241)), MColor(BLACK));
    
    MImage* closeImg    = new MImage(CLOSE_BTN);
    MImage* minimizeImg = new MImage(MINIMIZE_BTN);
    MImage* restoreImg  = new MImage(RESTORE_BTN);

    ImageButton* close    = new ImageButton(position,                                         MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), closeImg, closeFunc, (void*)this);
    ImageButton* minimize = new ImageButton(position + MPoint(size.x - 2 * TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), minimizeImg);
    ImageButton* restore  = new ImageButton(position + MPoint(size.x -     TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), restoreImg);

    Menu* topPanel = new Menu(position, topRect->getSize(), this, onMove);

    topPanel->registerObject(topRect);
    topPanel->registerObject(close);
    topPanel->registerObject(minimize);
    topPanel->registerObject(restore);

    subWindows->pushBack(topPanel);
}

void Window::createTestWindow() {
    isCreated = true;

    Window* subWin = new Window(position + MPoint(400, 100), MPoint(400, 400));
    subWin->col = MColor(GRAY);
    subWindows->pushBack(subWin);

    Window* subWin2 = new Window(position + MPoint(600, 200), MPoint(400, 400));
    subWin2->col = MColor(YELLOW);
    subWin2->isVisible = false;
    subWindows->pushBack(subWin2);

    Window* subWin3 = new Window(position + MPoint(300, 300), MPoint(400, 400));
    subWin3->isVisible = false;
    subWindows->pushBack(subWin3);
}

void Window::render(RenderTarget* renderTarget, RegionSet* regions) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    if (isVisible) renderTarget->drawRect(position, size, col, MColor(TRANSPARENT), regions);

    long listSize = long(subWindows->getSize());
    for (long i = listSize - 1; i >= 0; i--) {
        Widget* widget = (*subWindows)[i];
        if (widget && !widget->getExists()) {
            delete widget;
            subWindows->remove(i);
            listSize--;
        }
    }

    List<RegionSet*>* widgetRegs = createRegions();

    // draw
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) {
            if (widgetRegs) widget->render(renderTarget, (*widgetRegs)[i]);
            else            widget->render(renderTarget);
        }
    }

    delete widgetRegs;

    // RegionSet* inters1 = diff(MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(600, 200),  MPoint(400, 400)), MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(400, 100), MPoint(400, 400)));
    // RegionSet* inters2 = diff(MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(300, 300),  MPoint(400, 400)), MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(400, 100), MPoint(400, 400)));    

    // inters1->subtract(inters2);

    // if (inters1) {
    //     inters1->visualize(renderTarget);
    //     delete inters1;
    // }
    // else std::cout << "No inter\n";

    // if (inters2) {
    //     inters2->visualize(renderTarget);
    //     delete inters2;
    // }
    // else std::cout << "No inter\n";
}

void onMove(Window* window, MPoint newPos, MPoint oldPos) {
    ON_ERROR(!window, "Window pointer was null!",);

    window->move(newPos - oldPos);
}

void closeFunc(void* window) {
    ON_ERROR(!window, "Window pointer was null!",);

    ((Window*)window)->setExists(false);
}