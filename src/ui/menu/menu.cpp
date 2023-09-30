#include "menu.h"

Menu::Menu(MPoint _position) :
    Widget (_position)  {}

Menu::~Menu() { }

void Menu::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);
    ON_ERROR(!subWindows, "Buttons pointer was null!",);

    size_t listSize = subWindows->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Widget* btn = (*subWindows)[i];
        if (btn) btn->render(renderTarget);
    }
}