#include "menu.h"

Menu::Menu(MPoint _position, List<Button*>* _buttons) :
    Widget (_position),
    buttons(_buttons)  {}

Menu::~Menu() {
    buttons = nullptr;
}

void Menu::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target pointer was null!",);
    ON_ERROR(!buttons, "Buttons pointer was null!",);

    size_t listSize = buttons->getSize();
    for (size_t i = 0; i < listSize; i++) {
        Button* btn = (*buttons)[i];
        if (btn) btn->render(renderTarget);
    }
}