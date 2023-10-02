#include "includes.h"
#include "ui/window/window.h"

void testFunc(void*) {
    std::cout << "Clicked!\n";
}

Menu* createActionMenu() {
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);
    MFont  font  = MFont (DEFAULT_FONT);

    TextButton* fileBtn  = new TextButton(MPoint(0,                  TOP_PANE_SIZE), size, color, font, "File", testFunc);
    TextButton* editBtn  = new TextButton(MPoint(ACTION_BTN_LEN,     TOP_PANE_SIZE), size, color, font, "Edit");
    TextButton* viewBtn  = new TextButton(MPoint(ACTION_BTN_LEN * 2, TOP_PANE_SIZE), size, color, font, "View");
    TextButton* imageBtn = new TextButton(MPoint(ACTION_BTN_LEN * 3, TOP_PANE_SIZE), size, color, font, "Image");
    TextButton* colBtn   = new TextButton(MPoint(ACTION_BTN_LEN * 4, TOP_PANE_SIZE), size, color, font, "Color");

    Menu* actionMenu = new Menu(MPoint(0, TOP_PANE_SIZE));
    actionMenu->registerObject(fileBtn);
    actionMenu->registerObject(editBtn);
    actionMenu->registerObject(viewBtn);
    actionMenu->registerObject(imageBtn);
    actionMenu->registerObject(colBtn);

    return actionMenu;
}

MPoint globalCoordsToWindow(MPoint global, MPoint start) {
    return global - start;
}

void runMainCycle() {
    sf::RenderWindow window(sf::VideoMode(), "Photoshop", sf::Style::Fullscreen);
    window.setPosition(sf::Vector2i(0, 0));

    Menu* actions = createActionMenu();
    Window mainWindow = Window(              MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT), MPoint(window.getSize().x - 2 * MAIN_WIN_BRD_SHIFT, window.getSize().y - 2 * MAIN_WIN_BRD_SHIFT), actions);
    RenderTarget renderTarget = RenderTarget(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT), MPoint(window.getSize().x - 2 * MAIN_WIN_BRD_SHIFT, window.getSize().y - 2 * MAIN_WIN_BRD_SHIFT), &window);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // close if escape clicked
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                    mainWindow.onMousePressed(globalCoordsToWindow(MPoint(sf::Mouse::getPosition()), mainWindow.getPosition()), LEFT);
                break;
            }
        }

        window.clear();
        mainWindow.render(&renderTarget);
        window.display();
    }
}

int main() {
    runMainCycle();

    return 0;
}