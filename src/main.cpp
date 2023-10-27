#include "includes.h"
#include "ui/window/window.h"
#include "ui/submenu/submenu.h"
#include "events/events.h"
#include "ui/editbox/editbox.h"

enum Tools {
    BRUSH,
    LINE,
    RECT,
    ELLIPSE,
    CURVE,
    SPLINE,
    FILL
};

struct SubMenuArgs {
    FilterManager* filtManager;
    ToolManager  * manager;
    SubMenu      * subMenu;
    Tools          toolType;

    SubMenuArgs(FilterManager* _filtManager, ToolManager* _manager, SubMenu* _subMenu, Tools _toolType) :
        filtManager(_filtManager),
        manager    (_manager),
        subMenu    (_subMenu),
        toolType   (_toolType)      {}
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

struct ModalWindowArgs {
    Widget*        drawZone;
    SubMenu*       subMenu;
    EventManager*  evManager;
    FilterManager* filtManager;

    ModalWindowArgs(Widget* _drawZone, SubMenu* _subMenu, EventManager* _evManager, FilterManager* _filtManager) :
        drawZone   (_drawZone),
        subMenu    (_subMenu),
        evManager  (_evManager),
        filtManager(_filtManager)   {} 
};

void testFunc(void*) {
    std::cout << "Clicked!\n";
}

void openToolMenu(void* arg) {
    if (!arg) return;
    SubMenu* menu = (SubMenu*) arg;

    if (menu) menu->changeActivity();
}

void openBlurPicker(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modalWinArgs = (ModalWindowArgs*) arg;
    ModalWindow* modalWindow  = new ModalWindow(modalWinArgs->evManager, MPoint(300, 300), MPoint(500, 500), nullptr, modalWinArgs->filtManager, modalWinArgs->drawZone);

    EditBox* editBox = new EditBox(MPoint(300, 400), MPoint(300, 100), modalWindow, new MFont(DEFAULT_FONT));

    modalWindow->registerObject(editBox);
    modalWinArgs->drawZone->registerObject(modalWindow);
}

void changeBrightConst(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modalWinArgs = (ModalWindowArgs*) arg;

    modalWinArgs->filtManager->setLast  (new BrightnessFilter());
    modalWinArgs->filtManager->setActive(true);
    
    List<double> arguments = List<double>();
    arguments.pushBack(BRIGHTNESS_SHIFT);

    modalWinArgs->filtManager->getLast()->setParams(arguments);

    modalWinArgs->subMenu->changeActivity();
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
    case SPLINE:
        newTool = new Spline();
        break;
    case FILL:
        newTool = new FillTool();
        break;
    default:
        break;
    }

    menu->manager->setTool(newTool);
    menu->subMenu->changeActivity();
    menu->filtManager->setActive(false);
}

void chooseColor(void* arg) {
    if (!arg) return;
    ColPickerArgs* menu = (ColPickerArgs*) arg;

    menu->manager->setColor(menu->color);
    menu->subMenu->changeActivity();
}

SubMenu* createToolPicker(Window* _winPtr, FilterManager* _filtManager, ToolManager* _manager, List<SubMenuArgs*>& toolArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* toolMenu = new SubMenu(start + MPoint(ACTION_BTN_LEN    , 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 7 * TOP_PANE_SIZE), _winPtr);

    SubMenuArgs* brushArgs   = new SubMenuArgs(_filtManager, _manager, toolMenu, BRUSH);
    TextButton* brushBtn     = new TextButton(start + MPoint(ACTION_BTN_LEN, 2 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Brush", toolMenu, chooseTool, brushArgs);
  
    SubMenuArgs* lineArgs    = new SubMenuArgs(_filtManager, _manager, toolMenu, LINE);
    TextButton* lineBtn      = new TextButton(start + MPoint(ACTION_BTN_LEN, 3 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Line", toolMenu, chooseTool, lineArgs);
  
    SubMenuArgs* rectArgs    = new SubMenuArgs(_filtManager, _manager, toolMenu, RECT);
    TextButton* rectBtn      = new TextButton(start + MPoint(ACTION_BTN_LEN, 4 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Rectangle", toolMenu, chooseTool, rectArgs);

    SubMenuArgs* ellipseArgs = new SubMenuArgs(_filtManager, _manager, toolMenu, ELLIPSE);
    TextButton* ellipseBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN, 5 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Ellipse", toolMenu, chooseTool, ellipseArgs);

    SubMenuArgs* curveArgs   = new SubMenuArgs(_filtManager, _manager, toolMenu, CURVE);
    TextButton* curveBtn     = new TextButton(start + MPoint(ACTION_BTN_LEN, 6 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Curve", toolMenu, chooseTool, curveArgs);

    SubMenuArgs* splineArgs  = new SubMenuArgs(_filtManager, _manager, toolMenu, SPLINE);
    TextButton* splineBtn    = new TextButton(start + MPoint(ACTION_BTN_LEN, 7 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Spline", toolMenu, chooseTool, splineArgs);

    SubMenuArgs* fillArgs    = new SubMenuArgs(_filtManager, _manager, toolMenu, FILL);
    TextButton* fillBtn      = new TextButton(start + MPoint(ACTION_BTN_LEN, 8 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Fill", toolMenu, chooseTool, fillArgs);

    toolArgs.pushBack(brushArgs);
    toolArgs.pushBack(lineArgs);
    toolArgs.pushBack(rectArgs);
    toolArgs.pushBack(ellipseArgs);
    toolArgs.pushBack(curveArgs);
    toolArgs.pushBack(splineArgs);
    toolArgs.pushBack(fillArgs);

    toolMenu->registerObject(brushBtn);
    toolMenu->registerObject(lineBtn);
    toolMenu->registerObject(rectBtn);
    toolMenu->registerObject(ellipseBtn);
    toolMenu->registerObject(curveBtn);
    toolMenu->registerObject(splineBtn);
    toolMenu->registerObject(fillBtn);

    return toolMenu;
}

SubMenu* createColorPicker(Window* _winPtr, ToolManager* _manager, List<ColPickerArgs*>& colArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* colMenu  = new SubMenu(start + MPoint(ACTION_BTN_LEN * 2, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 9 * TOP_PANE_SIZE), _winPtr);

    ColPickerArgs* redArgs     = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Red));
    TextButton* redBtn         = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 2 * TOP_PANE_SIZE), size, MColor(sf::Color::Red), new MFont (DEFAULT_FONT), "Red", colMenu, chooseColor, redArgs);
    
    ColPickerArgs* greenArgs   = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Green));
    TextButton* greenBtn       = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 3 * TOP_PANE_SIZE), size, MColor(sf::Color::Green), new MFont (DEFAULT_FONT), "Green", colMenu, chooseColor, greenArgs);
    
    ColPickerArgs* yellowArgs  = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Yellow));
    TextButton* yellowBtn      = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 4 * TOP_PANE_SIZE), size, MColor(sf::Color::Yellow), new MFont (DEFAULT_FONT), "Yellow", colMenu, chooseColor, yellowArgs);
    
    ColPickerArgs* blueArgs    = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Blue));
    TextButton* blueBtn        = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 5 * TOP_PANE_SIZE), size, MColor(sf::Color::Blue), new MFont (DEFAULT_FONT), "Blue", colMenu, chooseColor, blueArgs);
    
    ColPickerArgs* blackArgs   = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Black));
    TextButton* blackBtn       = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 6 * TOP_PANE_SIZE), size, MColor(sf::Color::Black), new MFont (DEFAULT_FONT), "Black", colMenu, chooseColor, blackArgs);
    
    ColPickerArgs* cyanArgs    = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Cyan));
    TextButton* cyanBtn        = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 7 * TOP_PANE_SIZE), size, MColor(sf::Color::Cyan), new MFont (DEFAULT_FONT), "Cyan", colMenu, chooseColor, cyanArgs);
    
    ColPickerArgs* magentaArgs = new ColPickerArgs(_manager, colMenu, MColor(sf::Color::Magenta));    
    TextButton*    magentaBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 8 * TOP_PANE_SIZE), size, MColor(sf::Color::Magenta), new MFont (DEFAULT_FONT), "Magenta", colMenu, chooseColor, magentaArgs);

    colArgs.pushBack(redArgs);
    colArgs.pushBack(greenArgs);
    colArgs.pushBack(yellowArgs);
    colArgs.pushBack(blueArgs);
    colArgs.pushBack(blackArgs);
    colArgs.pushBack(cyanArgs);
    colArgs.pushBack(magentaArgs);

    colMenu->registerObject(redBtn);
    colMenu->registerObject(greenBtn);
    colMenu->registerObject(yellowBtn);
    colMenu->registerObject(blueBtn);
    colMenu->registerObject(blackBtn);
    colMenu->registerObject(cyanBtn);
    colMenu->registerObject(magentaBtn);

    return colMenu;
}

SubMenu* createFilterMenu(Widget* _drawZone, Window* _winPtr, ToolManager* _manager, FilterManager* _filtManager, EventManager* _evManager, List<ModalWindowArgs*>& modArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* filtMenu           = new SubMenu(start + MPoint(ACTION_BTN_LEN * 3, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 9 * TOP_PANE_SIZE), _winPtr);
    ModalWindowArgs* modWinArgs = new ModalWindowArgs(_drawZone, filtMenu, _evManager, _filtManager);

    TextButton* constBlurBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 2 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Blur (default)", filtMenu, changeBrightConst, modWinArgs);
    TextButton* customBlurBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 3 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Blur (custom)", filtMenu,  openBlurPicker,    modWinArgs);

    modArgs.pushBack(modWinArgs);

    filtMenu->registerObject(constBlurBtn);
    filtMenu->registerObject(customBlurBtn);

    return filtMenu;
}

Menu* createActionMenu(Widget* _drawZone, Window* _winPtr, ToolManager* _manager, FilterManager* _filtManager, EventManager* _evManager, List<SubMenuArgs*>& toolArgs, List<ColPickerArgs*>& colArgs, List<ModalWindowArgs*>& modArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    Menu*    actionMenu = new Menu(start + MPoint(0, TOP_PANE_SIZE), MPoint(4 * ACTION_BTN_LEN, TOP_PANE_SIZE), _winPtr);
    SubMenu* toolMenu   = createToolPicker (_winPtr, _filtManager, _manager, toolArgs);
    SubMenu* colMenu    = createColorPicker(_winPtr, _manager, colArgs);
    SubMenu* filtMenu   = createFilterMenu (_drawZone, _winPtr, _manager, _filtManager, _evManager, modArgs);

    TextButton* fileBtn   = new TextButton(start + MPoint(0,                  TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "New",    actionMenu, testFunc);
    TextButton* toolBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN,     TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Tools",  actionMenu, openToolMenu, toolMenu);
    TextButton* colBtn    = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Color",  actionMenu, openToolMenu, colMenu);
    TextButton* filterBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Filter", actionMenu, openToolMenu, filtMenu);

    actionMenu->registerObject(fileBtn);
    actionMenu->registerObject(toolBtn);
    actionMenu->registerObject(colBtn);
    actionMenu->registerObject(filterBtn);

    _winPtr->registerObject(toolMenu);
    _winPtr->registerObject(colMenu);
    _winPtr->registerObject(filtMenu);

    return actionMenu;
}

void runMainCycle() {
    sf::RenderWindow window(sf::VideoMode(), "Photoshop", sf::Style::Fullscreen);
    window.setPosition(sf::Vector2i(0, 0));
    RenderTarget renderTarget = RenderTarget(MPoint(0, 0), MPoint(1920, 1080), &window);

    Brush* defaultTool        = new Brush();
    ToolManager manager       = ToolManager(defaultTool, MColor(sf::Color::Red));
    FilterManager filtManager = FilterManager();

    Widget drawWidget  = Widget(MPoint(0, 0), MPoint(1920, 1080), nullptr);
    Window* mainWindow = new Window(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT), MPoint(1720, 880), &manager, &filtManager, &drawWidget);
    drawWidget.registerObject(mainWindow);

    EventManager eventBoy = EventManager();
    eventBoy.registerObject(mainWindow);

    // create bar with tool picker, color picker, and new window creator
    List<SubMenuArgs*> toolArgs; List<ColPickerArgs*> colArgs; List<ModalWindowArgs*> modArgs;
    Menu* actions = createActionMenu(&drawWidget, mainWindow, &manager, &filtManager, &eventBoy, toolArgs, colArgs, modArgs);
    mainWindow->setActions(actions);

    window.clear();
    drawWidget.render(&renderTarget);
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
                else eventBoy.onKeyPressed(MKeyboard(event.text.unicode));
                break;

            case sf::Event::KeyReleased:
                eventBoy.onKeyReleased(MKeyboard(event.text.unicode));
                break;

            case sf::Event::MouseButtonPressed: {
                renderTarget.getRenderTexture()->clear();
                if (event.mouseButton.button == sf::Mouse::Left) 
                    eventBoy.onMousePressed(MPoint(sf::Mouse::getPosition()), LEFT);
                if (event.mouseButton.button == sf::Mouse::Right)
                    eventBoy.onMousePressed(MPoint(sf::Mouse::getPosition()), RIGHT);

                drawWidget.render(&renderTarget);
                window.display();

                break;
            }
            case sf::Event::MouseButtonReleased: {
                renderTarget.getRenderTexture()->clear();
                if (event.mouseButton.button == sf::Mouse::Left)
                    eventBoy.onMouseReleased(MPoint(sf::Mouse::getPosition()), LEFT);
                if (event.mouseButton.button == sf::Mouse::Right)
                    eventBoy.onMouseReleased(MPoint(sf::Mouse::getPosition()), RIGHT);

                drawWidget.render(&renderTarget);
                window.display();

                break;
            }
            case sf::Event::MouseMoved: {
                renderTarget.getRenderTexture()->clear();
                eventBoy.onMouseMove(MPoint(sf::Mouse::getPosition()), LEFT);

                drawWidget.render(&renderTarget);
                window.display();

                break;
            }
            }
        }
    }

    for (size_t i = 0; i < toolArgs.getSize(); i++) delete toolArgs[i];
    for (size_t i = 0; i < colArgs .getSize(); i++) delete colArgs [i];
    for (size_t i = 0; i < modArgs .getSize(); i++) delete modArgs [i];
}

int main() {
    runMainCycle();

    return 0;
}