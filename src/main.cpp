#include "includes.h"
#include "ui/window/window.h"
#include "ui/submenu/submenu.h"

enum Tools {
    BRUSH,
    LINE,
    RECT,
    ELLIPSE,
    CURVE
};

struct SubMenuArgs {
    ToolManager* manager;
    SubMenu    * subMenu;
    Tools        toolType;

    SubMenuArgs(ToolManager* _manager, SubMenu* _subMenu, Tools _toolType) :
        manager (_manager),
        subMenu (_subMenu),
        toolType(_toolType) {}
};

struct ColPickerArgs {
    ToolManager* manager;
    SubMenu    * subMenu;
    MColor       color;

    ColPickerArgs(ToolManager* _manager, SubMenu* _subMenu, MColor _color) :
        manager(_manager),
        subMenu(_subMenu),
        color  (_color)     {}
};

void testFunc(void*) {
    std::cout << "Clicked!\n";
}

void openToolMenu(void* arg) {
    if (!arg) return;
    SubMenu* menu = (SubMenu*) arg;

    std::cout << menu->getPosition().x << '\n';

    if (menu) menu->changeActivity();
}

void chooseTool(void* arg) {
    if (!arg) return;
    SubMenuArgs* menu = (SubMenuArgs*) arg;

    delete menu->manager->getTool();

    Tool* newTool = nullptr;

    switch (menu->toolType)
    {
    case BRUSH:
        newTool = new Brush();
        break;
    case LINE:
        newTool = new LineTool();
        break;
    case RECT:
        newTool = new SquareTool();
        break;
    case ELLIPSE:
        newTool = new EllipseTool();
        break;
    case CURVE:
        newTool = new CurveTool();
        break;
    default:
        break;
    }

    menu->manager->setTool(newTool);
    menu->subMenu->changeActivity();
}

void chooseColor(void* arg) {
    if (!arg) return;
    ColPickerArgs* menu = (ColPickerArgs*) arg;

    menu->manager->setColor(menu->color);
    menu->subMenu->changeActivity();
}

Menu* createActionMenu(Window* _winPtr, ToolManager* _manager) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    Menu* actionMenu = new Menu(start + MPoint(0, TOP_PANE_SIZE), MPoint(3 * ACTION_BTN_LEN, TOP_PANE_SIZE), _winPtr);

    SubMenu* toolMenu = new SubMenu(start + MPoint(ACTION_BTN_LEN    , 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 7 * TOP_PANE_SIZE), _winPtr);
    SubMenu* colMenu  = new SubMenu(start + MPoint(ACTION_BTN_LEN * 2, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 9 * TOP_PANE_SIZE), _winPtr);

    SubMenuArgs* brushArgs = new SubMenuArgs(_manager, toolMenu, BRUSH);
    TextButton* brushBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN, 2 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Brush", toolMenu, chooseTool, brushArgs);

    SubMenuArgs* lineArgs = new SubMenuArgs(_manager, toolMenu, LINE);
    TextButton* lineBtn    = new TextButton(start + MPoint(ACTION_BTN_LEN, 3 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Line", toolMenu, chooseTool, lineArgs);

    SubMenuArgs* rectArgs = new SubMenuArgs(_manager, toolMenu, RECT);
    TextButton* rectBtn    = new TextButton(start + MPoint(ACTION_BTN_LEN, 4 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Rectangle", toolMenu, chooseTool, rectArgs);

    SubMenuArgs* ellipseArgs = new SubMenuArgs(_manager, toolMenu, ELLIPSE);
    TextButton* ellipseBtn = new TextButton(start + MPoint(ACTION_BTN_LEN, 5 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Ellipse", toolMenu, chooseTool, ellipseArgs);

    SubMenuArgs* curveArgs = new SubMenuArgs(_manager, toolMenu, CURVE);
    TextButton* curveBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN, 6 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Curve", toolMenu, chooseTool, curveArgs);

    toolMenu->registerObject(brushBtn);
    toolMenu->registerObject(lineBtn);
    toolMenu->registerObject(rectBtn);
    toolMenu->registerObject(ellipseBtn);
    toolMenu->registerObject(curveBtn);

    ColPickerArgs* redArgs = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Red));
    TextButton* redBtn     = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 2 * TOP_PANE_SIZE), size, MColor(sf::Color::Red), new MFont (DEFAULT_FONT), "Red", colMenu, chooseColor, redArgs);
    
    ColPickerArgs* greenArgs = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Green));
    TextButton* greenBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 3 * TOP_PANE_SIZE), size, MColor(sf::Color::Green), new MFont (DEFAULT_FONT), "Green", colMenu, chooseColor, greenArgs);
    
    ColPickerArgs* yellowArgs = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Yellow));
    TextButton* yellowBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 4 * TOP_PANE_SIZE), size, MColor(sf::Color::Yellow), new MFont (DEFAULT_FONT), "Yellow", colMenu, chooseColor, yellowArgs);
    
    ColPickerArgs* blueArgs = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Blue));
    TextButton* blueBtn    = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 5 * TOP_PANE_SIZE), size, MColor(sf::Color::Blue), new MFont (DEFAULT_FONT), "Blue", colMenu, chooseColor, blueArgs);
    
    ColPickerArgs* blackArgs = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Black));
    TextButton* blackBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 6 * TOP_PANE_SIZE), size, MColor(sf::Color::Black), new MFont (DEFAULT_FONT), "Black", colMenu, chooseColor, blackArgs);
    
    ColPickerArgs* cyanArgs = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Cyan));
    TextButton* cyanBtn    = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 7 * TOP_PANE_SIZE), size, MColor(sf::Color::Cyan), new MFont (DEFAULT_FONT), "Cyan", colMenu, chooseColor, cyanArgs);
    
    ColPickerArgs* magentaArgs = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Magenta));    
    TextButton* magentaBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 8 * TOP_PANE_SIZE), size, MColor(sf::Color::Magenta), new MFont (DEFAULT_FONT), "Magenta", colMenu, chooseColor, magentaArgs);

    colMenu->registerObject(redBtn);
    colMenu->registerObject(greenBtn);
    colMenu->registerObject(yellowBtn);
    colMenu->registerObject(blueBtn);
    colMenu->registerObject(blackBtn);
    colMenu->registerObject(cyanBtn);
    colMenu->registerObject(magentaBtn);

    TextButton* fileBtn  = new TextButton(start + MPoint(0,                  TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "New", actionMenu, testFunc);
    TextButton* toolBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN,     TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Tools", actionMenu, openToolMenu, toolMenu);
    TextButton* colBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Color", actionMenu, openToolMenu, colMenu);

    actionMenu->registerObject(fileBtn);
    actionMenu->registerObject(toolBtn);
    actionMenu->registerObject(colBtn);

    _winPtr->registerObject(toolMenu);
    _winPtr->registerObject(colMenu);

    return actionMenu;
}

void runMainCycle() {
    sf::RenderWindow window(sf::VideoMode(), "Photoshop", sf::Style::Fullscreen);
    window.setPosition(sf::Vector2i(0, 0));
    RenderTarget renderTarget = RenderTarget(MPoint(0, 0), MPoint(1920, 1080), &window);

    Brush* defaultTool = new Brush();
    ToolManager manager = ToolManager(defaultTool, MColor(sf::Color::Red));

    Window mainWindow = Window(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT), MPoint(1720, 880), &manager, nullptr);
    Menu* actions = createActionMenu(&mainWindow, &manager);
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
                renderTarget.getRenderTexture()->clear();
                mainWindow.render(&renderTarget);
                if (event.mouseButton.button == sf::Mouse::Left) 
                    mainWindow.onMousePressed(MPoint(sf::Mouse::getPosition()), LEFT);
                if (event.mouseButton.button == sf::Mouse::Right)
                    mainWindow.onMousePressed(MPoint(sf::Mouse::getPosition()), RIGHT);

                mainWindow.render(&renderTarget);
                window.display();

                break;
            }
            case sf::Event::MouseButtonReleased: {
                renderTarget.getRenderTexture()->clear();
                if (event.mouseButton.button == sf::Mouse::Left)
                    mainWindow.onMouseReleased(MPoint(sf::Mouse::getPosition()), LEFT);
                if (event.mouseButton.button == sf::Mouse::Right)
                    mainWindow.onMouseReleased(MPoint(sf::Mouse::getPosition()), RIGHT);

                mainWindow.render(&renderTarget);
                window.display();

                break;
            }
            case sf::Event::MouseMoved: {
                renderTarget.getRenderTexture()->clear();
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