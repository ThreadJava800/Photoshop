#include "window.h"

bool isCreated = false;

Window::Window(MPoint _position, MPoint _size) :
    Widget (_position),
    size   (_size),
    actions(nullptr)    {
        createTopPanel();
        if (!isCreated) createTestWindow();
    }

Window::Window(MPoint _position, MPoint _size, Menu* _actions) :
    Widget (_position),
    size   (_size),
    actions(_actions)   {
        createTopPanel();
        if (!isCreated) createTestWindow();
        subWindows->pushBack(actions);
    }

Window::~Window() {}

void Window::createTopPanel() {
    Rectangle*   topRect = new Rectangle(position, MPoint(size.x, TOP_PANE_SIZE), MColor(sf::Color(161, 200, 241)), MColor(BLACK));
    
    MImage* closeImg    = new MImage(CLOSE_BTN);
    MImage* minimizeImg = new MImage(MINIMIZE_BTN);
    MImage* restoreImg  = new MImage(RESTORE_BTN);

    ImageButton* close    = new ImageButton(position,                                         MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), closeImg);
    ImageButton* minimize = new ImageButton(position + MPoint(size.x - 2 * TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), minimizeImg);
    ImageButton* restore  = new ImageButton(position + MPoint(size.x -     TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), restoreImg);

    Menu* topPanel = new Menu(position);

    topPanel->registerObject(topRect);
    topPanel->registerObject(close);
    topPanel->registerObject(minimize);
    topPanel->registerObject(restore);

    subWindows->pushBack(topPanel);
}

void Window::createTestWindow() {
    isCreated = true;

    Window* subWin = new Window(position + MPoint(300, 300), MPoint(400, 400));
    subWindows->pushBack(subWin);
}

void Window::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL), MColor(GRAY));

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) widget->render(renderTarget);
    }
}