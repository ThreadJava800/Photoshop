#include "window.h"

bool isCreated = false;

Window::Window(MPoint _position, MPoint _size) :
    Widget (_position),
    size   (_size),
    actions(nullptr)   {
        createTopPanel();
        if (!isCreated) createTestWindow();
    }

Window::Window(MPoint _position, MPoint _size, Menu* _actions) :
    Widget (_position),
    size   (_size),
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
    subWindows->pushBack(subWin);

    Window* subWin2 = new Window(position + MPoint(600, 200), MPoint(400, 400));
    subWindows->pushBack(subWin2);
}

void Window::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL), MColor(GRAY));

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

    // RegionSet* inters = diff(renderTarget, MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(400, 0),  MPoint(400, 400)), MathRectangle(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT) + MPoint(600, 200), MPoint(400, 400)));
    // if (inters) {
    //     inters->visualize(renderTarget);
    //     delete inters;
    // }
    // else std::cout << "No inter\n";

    // visualise renderSets
    //List<RegionSet*>* res = getRegionSet(renderTarget);
    // listSize = res->getSize();
    // for (size_t i = 0; i < listSize; i++) {
    //     (*res)[i]->visualize(renderTarget);
                

    //     delete (*res)[i];
    // }
    // delete res;
}

List<RegionSet*>* Window::getRegionSet(RenderTarget* renderTarget) {
    List<RegionSet*>* regionSets = new List<RegionSet*>();

    RegionSet* regSet = new RegionSet();
    regSet->regionFromObject(renderTarget, this, size);
    regionSets->pushBack(regSet);

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget && widget->getExists()) regionSets->pushBack(widget->getRegionSet(renderTarget));
    }

    return regionSets;
}

void onMove(Window* window, MPoint newPos, MPoint oldPos) {
    ON_ERROR(!window, "Window pointer was null!",);

    window->move(newPos - oldPos);
}

void closeFunc(void* window) {
    ON_ERROR(!window, "Window pointer was null!",);

    ((Window*)window)->setExists(false);
}