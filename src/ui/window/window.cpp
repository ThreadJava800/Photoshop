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
    // Button btn = Button();
}

void Window::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);

    renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL));

    if (actions) actions->render(renderTarget);
    drawTopPanel(renderTarget);
}