#include "window.h"

Window::Window(MPoint _position, MPoint _size) :
    Widget (_position),
    size   (_size),
    actions(nullptr)    {}

Window::Window(MPoint _position, MPoint _size, Menu* _actions) :
    Widget (_position),
    size   (_size),
    actions(_actions)   {}

Window::~Window() {
    actions = nullptr;
}

void Window::drawTopPanel(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(MPoint(0, 0), MPoint(size.x, CLOSE_BTN_SIZE), MColor(sf::Color(161, 200, 241)));

    static ImageButton close = ImageButton(MPoint(0, 0), MPoint(CLOSE_BTN_SIZE, CLOSE_BTN_SIZE), MImage(CLOSE_BTN));
    close.render(renderTarget);

    static ImageButton minimize = ImageButton(MPoint(size.x - 2 * CLOSE_BTN_SIZE, 0), MPoint(CLOSE_BTN_SIZE, CLOSE_BTN_SIZE), MImage(MINIMIZE_BTN));
    minimize.render(renderTarget);

    static ImageButton restore  = ImageButton(MPoint(size.x - CLOSE_BTN_SIZE, 0), MPoint(CLOSE_BTN_SIZE, CLOSE_BTN_SIZE), MImage(RESTORE_BTN));
    restore.render(renderTarget);
}

void Window::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(MPoint(0, 0), size, MColor(DEFAULT_BACK_COL));

    if (actions) actions->render(renderTarget);
    drawTopPanel(renderTarget);
}