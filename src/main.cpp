#include "includes.h"
#include "ui/window/window.h"

void testFunc(void*) {
    std::cout << "Clicked!\n";
}

Menu* createActionMenu() {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    TextButton* fileBtn  = new TextButton(start + MPoint(0,                  TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "File", testFunc);
    TextButton* editBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN,     TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Edit");
    TextButton* viewBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "View");
    TextButton* imageBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Image");
    TextButton* colBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN * 4, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Color");

    Menu* actionMenu = new Menu(start + MPoint(0, TOP_PANE_SIZE));
    actionMenu->registerObject(fileBtn);
    actionMenu->registerObject(editBtn);
    actionMenu->registerObject(viewBtn);
    actionMenu->registerObject(imageBtn);
    actionMenu->registerObject(colBtn);

    return actionMenu;
}

void runMainCycle() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Photoshop");
    window.setPosition(sf::Vector2i(0, 0));

    Menu* actions = createActionMenu();
    Window mainWindow = Window(              MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT), MPoint(1720, 880), actions);
    RenderTarget renderTarget = RenderTarget(MPoint(0, 0), MPoint(1920, 1080), &window);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
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
                    mainWindow.onMousePressed(MPoint(sf::Mouse::getPosition()), LEFT);
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