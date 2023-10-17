#include "includes.h"
#include "ui/window/window.h"

void testFunc(void*) {
    std::cout << "Clicked!\n";
}

Menu* createActionMenu(Window* _winPtr) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    Menu* actionMenu = new Menu(start + MPoint(0, TOP_PANE_SIZE), MPoint(5 * ACTION_BTN_LEN, TOP_PANE_SIZE), _winPtr);

    TextButton* fileBtn  = new TextButton(start + MPoint(0,                  TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "File", actionMenu, testFunc);
    TextButton* editBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN,     TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Edit", actionMenu);
    TextButton* viewBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "View", actionMenu);
    TextButton* imageBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Image", actionMenu);
    TextButton* colBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN * 4, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Color", actionMenu);

    actionMenu->registerObject(fileBtn);
    actionMenu->registerObject(editBtn);
    actionMenu->registerObject(viewBtn);
    actionMenu->registerObject(imageBtn);
    actionMenu->registerObject(colBtn);

    return actionMenu;
}

void runMainCycle() {
    sf::RenderWindow window(sf::VideoMode(), "Photoshop", sf::Style::Fullscreen);
    window.setPosition(sf::Vector2i(0, 0));
    RenderTarget renderTarget = RenderTarget(MPoint(0, 0), MPoint(1920, 1080), &window);

    EllipseTool defaultTool = EllipseTool();
    ToolManager manager = ToolManager(&defaultTool, MColor(sf::Color::Red));

    Window mainWindow = Window(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT), MPoint(1720, 880), &manager, nullptr);
    Menu* actions = createActionMenu(&mainWindow);
    mainWindow.setActions(actions);

    window.clear();
    mainWindow.render(&renderTarget);
    window.display();

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
            case sf::Event::MouseButtonPressed: {
                // renderTarget.getRenderTexture()->clear();
                // window.clear();
                mainWindow.render(&renderTarget);
                if (event.mouseButton.button == sf::Mouse::Left) 
                    mainWindow.onMousePressed(MPoint(sf::Mouse::getPosition()), LEFT);

                mainWindow.render(&renderTarget);
                window.display();

                break;
            }
            case sf::Event::MouseButtonReleased: {
                // renderTarget.getRenderTexture()->clear();
                // window.clear();
                if (event.mouseButton.button == sf::Mouse::Left)
                    mainWindow.onMouseReleased(MPoint(sf::Mouse::getPosition()), LEFT);

                mainWindow.render(&renderTarget);
                window.display();

                break;
            }
            case sf::Event::MouseMoved: {
                // renderTarget.getRenderTexture()->clear();
                // window.clear();
                mainWindow.onMouseMove(MPoint(sf::Mouse::getPosition()), LEFT);

                mainWindow.render(&renderTarget);
                window.display();

                break;
            }
            }
        }
    }
}

int main() {
    runMainCycle();

    return 0;
}