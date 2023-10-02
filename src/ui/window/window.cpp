#include "window.h"

Window::Window(MPoint _position, MPoint _size) :
    Widget (_position),
    size   (_size),
    actions(nullptr)    {
        createTopPanel();
    }

Window::Window(MPoint _position, MPoint _size, Menu* _actions) :
    Widget (_position),
    size   (_size),
    actions(_actions)   {
        createTopPanel();
        subWindows->pushBack(actions);
    }

Window::~Window() {
    actions = nullptr;
}

void Window::createTopPanel() {
    Rectangle*   topRect = new Rectangle(MPoint(0, 0), MPoint(size.x, TOP_PANE_SIZE), MColor(sf::Color(161, 200, 241)));
    
    ImageButton* close    = new ImageButton(MPoint(0, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), MImage(CLOSE_BTN));
    ImageButton* minimize = new ImageButton(MPoint(size.x - 2 * TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), MImage(MINIMIZE_BTN));
    ImageButton* restore  = new ImageButton(MPoint(size.x - TOP_PANE_SIZE, 0), MPoint(TOP_PANE_SIZE, TOP_PANE_SIZE), MImage(RESTORE_BTN));

    Menu* topPanel = new Menu(MPoint(0, 0));

    topPanel->registerObject(topRect);
    topPanel->registerObject(close);
    topPanel->registerObject(minimize);
    topPanel->registerObject(restore);

    subWindows->pushBack(topPanel);
}

void Window::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(MPoint(0, 0), size, MColor(DEFAULT_BACK_COL));

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* widget = (*subWindows)[i];
        if (widget) widget->render(renderTarget);
    }
}