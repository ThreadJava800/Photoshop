#include <chrono>

#include "includes.h"
#include "ui/window/window.h"
#include "ui/submenu/submenu.h"
#include "events/events.h"
#include "ui/editbox/editbox.h"
#include "../libs/multimedia/eventInterlayer.h"
#include "ui/shapes/shapes.h"

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
    SubMenu*       subMenuChild;
    EventManager*  evManager;
    FilterManager* filtManager;
    WindowManager* winManager;
    Window       * curWindow;
    EditBoxModal * editBoxModal;
    double         saturCoeff;

    ModalWindowArgs(Widget* _drawZone, SubMenu* _subMenu, EventManager* _evManager, FilterManager* _filtManager, WindowManager* _winManager, double _saturCoeff = 1.0) :
        drawZone   (_drawZone),
        subMenu    (_subMenu),
        evManager  (_evManager),
        filtManager(_filtManager),
        winManager (_winManager),
        saturCoeff (_saturCoeff)   {} 
};

void testFunc(void*) {
    std::cout << "Clicked!\n";
}

void openToolMenu(void* arg) {
    if (!arg) return;
    SubMenu* menu = (SubMenu*) arg;

    if (menu) menu->changeActivity();
}

void closeModal(void* arg) {
    if (!arg) {
        return;
    }

    EditBoxModal* modWindow = (EditBoxModal*) arg;

    plugin::FilterI*      filter       = modWindow->getFiltManager()->getLast();
    List<EditBox*>*       editBoxes    = modWindow->getEditBoxes();
    size_t                editBoxesCnt = editBoxes->getSize();
    plugin::Array<double> doubleArgs;

    doubleArgs.size = editBoxesCnt;
    doubleArgs.data = new double[editBoxesCnt];

    for (size_t i = 0; i < editBoxesCnt; i++) {
        double doubleArg = atof((*editBoxes)[i]->getText());
        doubleArgs.data[i] = doubleArg;
    }

    filter->setParams(doubleArgs);
    modWindow->getFiltManager()->setActive(true);
}

void openBlurPicker(void* arg) {
    if (!arg) return;

    ModalWindowArgs*   modalWinArgs = (ModalWindowArgs*) arg;

    modalWinArgs->filtManager->setFilter(new BrightnessFilter());

    plugin::Array<const char*> paramNames  = modalWinArgs->filtManager->getLast()->getParamNames();
    EditBoxModal*              modalWindow = new EditBoxModal(modalWinArgs->evManager, MPoint(300, 300), MPoint(500, 500), "Brightness", nullptr, modalWinArgs->filtManager, modalWinArgs->drawZone, paramNames);
    modalWindow->setOnDestroy(closeModal);
    modalWindow->setDestrArgs(modalWindow);

    // add edit box
    EditBox* editBox = new EditBox(MPoint(300, 400), MPoint(300, 50), modalWindow, new MFont(DEFAULT_FONT), NUMBERS_ONLY);
    modalWindow->addEditBox(editBox);

    // TextButton* okBtn = new TextButton(MPoint(300, 500), MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT), color, new MFont (DEFAULT_FONT), "OK", modalWindow, closeModal, lineArgs);

    // add modalWindow 
    modalWinArgs->drawZone->registerObject(modalWindow);

    // close subMenu
    modalWinArgs->subMenu->changeActivity();
}

void changeBrightConst(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modalWinArgs = (ModalWindowArgs*) arg;

    modalWinArgs->filtManager->setFilter(new BrightnessFilter());
    modalWinArgs->filtManager->setActive(true);
    
    plugin::Array<double> arguments;
    arguments.size    = 1;
    arguments.data    = new double[arguments.size];
    arguments.data[0] = BRIGHTNESS_SHIFT;

    modalWinArgs->filtManager->getLast()->setParams(arguments);

    modalWinArgs->subMenu->changeActivity();
}

void monochromeFilter(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modalWinArgs = (ModalWindowArgs*) arg;

    modalWinArgs->filtManager->setFilter(new MonochromeFilter());
    modalWinArgs->filtManager->setActive(true);

    modalWinArgs->subMenu->changeActivity();
}

void saturationFilter(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modalWinArgs = (ModalWindowArgs*) arg;

    modalWinArgs->filtManager->setFilter(new ColorfulnessFilter());
    modalWinArgs->filtManager->setActive(true);

    plugin::Array<double> params;
    params.size    = 1;
    params.data    = new double[params.size];
    params.data[0] = modalWinArgs->saturCoeff;

    modalWinArgs->filtManager->getLast()->setParams(params);

    modalWinArgs->subMenu->changeActivity();
}

void lastFilter(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modalWinArgs = (ModalWindowArgs*) arg;

    modalWinArgs->filtManager->setActive(true);
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

    menu->manager->setColor(menu->color.toPlColor());
    menu->subMenu->changeActivity();
}

void saveCanvas(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modWinArgs = (ModalWindowArgs*) arg;
    EditBoxModal   * modWindow  = modWinArgs->editBoxModal;

    List<EditBox*>* editBoxes = modWindow->getEditBoxes();
    char*     fileName  = (*editBoxes)[0]->getText();

    MImage texture = MImage(modWinArgs->curWindow->getCanvas()->getTexture());
    bool res = texture.saveToFile(fileName);
    if (!res) return;

    modWinArgs->curWindow->setName(fileName);

    WindowManager* _winManager  = modWinArgs->winManager;
    size_t          winCnt      = modWinArgs->subMenuChild->getWindows()->getSize();
    List<Widget *>* subMenuBtns = modWinArgs->subMenuChild->getWindows();

    for (size_t i = 0; i < winCnt; i++) {
        const char* winName = (*_winManager->getCanvasWindows())[i]->getName();
        ((TextButton*)(*subMenuBtns)[i])->setText(winName);
    }
}

void saveBtnFunc(void* arg) {
    if (!arg) return;

    ModalWindowArgs*   modalWinArgs = (ModalWindowArgs*) arg;

    plugin::Array<const char*> paramNames;
    paramNames.size    = 1;
    paramNames.data    = new const char*[paramNames.size];
    paramNames.data[0] = "Enter filename";
    EditBoxModal* modalWindow  = new EditBoxModal(modalWinArgs->evManager, MPoint(300, 300), MPoint(500, 500), "Choose filename", nullptr, modalWinArgs->filtManager, modalWinArgs->drawZone, paramNames);
    
    modalWinArgs->editBoxModal = modalWindow;
    
    modalWindow->setOnDestroy(saveCanvas);
    modalWindow->setDestrArgs(modalWinArgs);

    EditBox* editBox = new EditBox(MPoint(300, 400), MPoint(300, 50), modalWindow, new MFont(DEFAULT_FONT), ALL_CHARACTER);

    modalWindow->addEditBox(editBox);
    modalWinArgs->drawZone->registerObject(modalWindow);

    modalWinArgs->subMenuChild->changeActivity();
    modalWinArgs->subMenu     ->changeActivity();
}

SubMenu* createToolPicker(Widget* _winPtr, FilterManager* _filtManager, ToolManager* _manager, List<SubMenuArgs*>& toolArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* toolMenu = new SubMenu(start + MPoint(ACTION_BTN_LEN    , 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 7 * TOP_PANE_SIZE), _winPtr);
    // SubMenu* testSub  = new SubMenu(start + MPoint(ACTION_BTN_LEN    , 2 * TOP_PANE_SIZE) + MPoint(2 * ACTION_BTN_LEN, 4 * TOP_PANE_SIZE), MPoint(100, 100), toolMenu);
    // testSub->setVisible(true);

    // toolMenu->registerObject(testSub);

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

SubMenu* createColorPicker(Widget* _winPtr, ToolManager* _manager, List<ColPickerArgs*>& colArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* colMenu  = new SubMenu(start + MPoint(ACTION_BTN_LEN * 2, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 9 * TOP_PANE_SIZE), _winPtr);

    ColPickerArgs* redArgs     = new ColPickerArgs(_manager, colMenu, MColor::RED);
    TextButton* redBtn         = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 2 * TOP_PANE_SIZE), size, MColor::RED, new MFont (DEFAULT_FONT), "Red", colMenu, chooseColor, redArgs);
    
    ColPickerArgs* greenArgs   = new ColPickerArgs(_manager, colMenu, MColor::GREEN);
    TextButton* greenBtn       = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 3 * TOP_PANE_SIZE), size, MColor::GREEN, new MFont (DEFAULT_FONT), "Green", colMenu, chooseColor, greenArgs);
    
    ColPickerArgs* yellowArgs  = new ColPickerArgs(_manager, colMenu, MColor::YELLOW);
    TextButton* yellowBtn      = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 4 * TOP_PANE_SIZE), size, MColor::YELLOW, new MFont (DEFAULT_FONT), "Yellow", colMenu, chooseColor, yellowArgs);
    
    ColPickerArgs* blueArgs    = new ColPickerArgs(_manager, colMenu, MColor::BLUE);
    TextButton* blueBtn        = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 5 * TOP_PANE_SIZE), size, MColor::BLUE, new MFont (DEFAULT_FONT), "Blue", colMenu, chooseColor, blueArgs);
    
    ColPickerArgs* blackArgs   = new ColPickerArgs(_manager, colMenu, MColor::BLACK);
    TextButton* blackBtn       = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 6 * TOP_PANE_SIZE), size, MColor::BLACK, new MFont (DEFAULT_FONT), "Black", colMenu, chooseColor, blackArgs);
    
    ColPickerArgs* cyanArgs    = new ColPickerArgs(_manager, colMenu, MColor::CYAN);
    TextButton* cyanBtn        = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 7 * TOP_PANE_SIZE), size, MColor::CYAN, new MFont (DEFAULT_FONT), "Cyan", colMenu, chooseColor, cyanArgs);
    
    ColPickerArgs* magentaArgs = new ColPickerArgs(_manager, colMenu, MColor::MAGENTA);    
    TextButton*    magentaBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 8 * TOP_PANE_SIZE), size, MColor::MAGENTA, new MFont (DEFAULT_FONT), "Magenta", colMenu, chooseColor, magentaArgs);

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

SubMenu* createFilterMenu(Widget* _drawZone, Widget* _winPtr, ToolManager* _manager, FilterManager* _filtManager, EventManager* _evManager, List<ModalWindowArgs*>& modArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* filtMenu           = new SubMenu(start + MPoint(ACTION_BTN_LEN * 3, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 9 * TOP_PANE_SIZE), _winPtr);
    ModalWindowArgs* modWinArgs = new ModalWindowArgs(_drawZone, filtMenu, _evManager, _filtManager, nullptr, ColorfulnessFilter::SATUR_UP);

    TextButton* lastBtn       = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 2 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Last used",      filtMenu,  lastFilter,        modWinArgs);
    TextButton* constBlurBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 3 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Blur (default)", filtMenu,  changeBrightConst, modWinArgs);
    TextButton* customBlurBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 4 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Blur (custom)",  filtMenu,  openBlurPicker,    modWinArgs);
    TextButton* monochromeBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 5 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Monochrome",     filtMenu,  monochromeFilter,  modWinArgs);
    TextButton* satUpBtn      = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 6 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Satur. (up)",   filtMenu,  saturationFilter,  modWinArgs);
    
    ModalWindowArgs* satDownArgs = new ModalWindowArgs(_drawZone, filtMenu, _evManager, _filtManager, nullptr, ColorfulnessFilter::SATUR_DOWN);
    TextButton* satDownBtn       = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 7 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Satur. (down)",   filtMenu,  saturationFilter,  satDownArgs);

    modArgs.pushBack(modWinArgs);
    modArgs.pushBack(satDownArgs);

    filtMenu->registerObject(lastBtn);
    filtMenu->registerObject(constBlurBtn);
    filtMenu->registerObject(customBlurBtn);
    filtMenu->registerObject(monochromeBtn);
    filtMenu->registerObject(satUpBtn);
    filtMenu->registerObject(satDownBtn);

    return filtMenu;
}

SubMenu* createFileMenu(Widget* _drawZone, Widget* _winPtr, ToolManager* _manager, FilterManager* _filtManager, EventManager* _evManager, WindowManager* _winManager, List<ModalWindowArgs*>& modArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* fileMenu   = new SubMenu(start + MPoint(0, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 3 * TOP_PANE_SIZE), _winPtr);
    size_t   winCnt     = _winManager->getCanvasWindows()->getSize();
    ModalWindowArgs* modWinArgs = new ModalWindowArgs(_drawZone, fileMenu, _evManager, _filtManager, _winManager);

    MPoint chooseMenuPos = fileMenu->getPosition() + MPoint(fileMenu->getSize().x, 0);
    SubMenu* chooseMenu  = new SubMenu(chooseMenuPos, MPoint(fileMenu->getSize().x, TOP_PANE_SIZE * winCnt), fileMenu);
    
    for (size_t i = 0; i < winCnt; i++) {
        const char* winName = (*_winManager->getCanvasWindows())[i]->getName();

        ModalWindowArgs* fileBtnArgs = new ModalWindowArgs(_drawZone, fileMenu, _evManager, _filtManager, _winManager);
        fileBtnArgs->curWindow    = (*_winManager->getCanvasWindows())[i];
        fileBtnArgs->subMenuChild = chooseMenu;
        modArgs.pushBack(fileBtnArgs);
        
        TextButton* fileBtn = new TextButton(chooseMenuPos + MPoint(0, i * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT), DEFAULT_BACK_COL, new MFont(DEFAULT_FONT), winName, chooseMenu, saveBtnFunc, fileBtnArgs);
        chooseMenu->registerObject(fileBtn);
    }

    TextButton* saveBtn = new TextButton(start + MPoint(0, 2 * TOP_PANE_SIZE), size, color, new MFont(DEFAULT_FONT), "Save", fileMenu, openToolMenu, chooseMenu);

    fileMenu->registerObject(saveBtn);
    fileMenu->registerObject(chooseMenu);

    modArgs.pushBack(modWinArgs);

    return fileMenu;
}

Menu* createActionMenu(Widget* _drawZone, Widget* _winPtr, ToolManager* _manager, FilterManager* _filtManager, EventManager* _evManager, WindowManager* _winManager, List<SubMenuArgs*>& toolArgs, List<ColPickerArgs*>& colArgs, List<ModalWindowArgs*>& modArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    Menu*    actionMenu = new Menu(start + MPoint(0, TOP_PANE_SIZE), MPoint(4 * ACTION_BTN_LEN, TOP_PANE_SIZE), _winPtr);
    SubMenu* fileMenu   = createFileMenu   (_drawZone, actionMenu, _manager, _filtManager, _evManager, _winManager, modArgs);
    SubMenu* toolMenu   = createToolPicker (actionMenu, _filtManager, _manager, toolArgs);
    SubMenu* colMenu    = createColorPicker(actionMenu, _manager, colArgs);
    SubMenu* filtMenu   = createFilterMenu (_drawZone, actionMenu, _manager, _filtManager, _evManager, modArgs);

    TextButton* fileBtn   = new TextButton(start + MPoint(0,                  TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "File",   actionMenu, openToolMenu, fileMenu);
    TextButton* toolBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN,     TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Tools",  actionMenu, openToolMenu, toolMenu);
    TextButton* colBtn    = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Color",  actionMenu, openToolMenu, colMenu);
    TextButton* filterBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Filter", actionMenu, openToolMenu, filtMenu);

    actionMenu->registerObject(fileBtn);
    actionMenu->registerObject(toolBtn);
    actionMenu->registerObject(colBtn);
    actionMenu->registerObject(filterBtn);

    actionMenu->registerObject(fileMenu);
    actionMenu->registerObject(toolMenu);
    actionMenu->registerObject(colMenu);
    actionMenu->registerObject(filtMenu);

    return actionMenu;
}

Window* createPickerWindow(Window* _parent, ToolManager* _toolMan, FilterManager* _filtManager) {
    MPoint start = MPoint(2 * MAIN_WIN_BRD_SHIFT, 2 * MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(400, 400);
    MColor color = MColor(DEFAULT_COLOR);

    MPoint btnSize = MPoint(PICKER_BTN_SIZE, PICKER_BTN_SIZE);

    Window* window = new Window(start, size, "Choose tool", _toolMan, _filtManager, nullptr, false, _parent);

    MImage     * brLogo   = new MImage(BRUSH_BTN); 
    ImageButton* brush    = new ImageButton(start + MPoint(10, 40), btnSize, brLogo, window);

    MImage     * rectLogo = new MImage(RECT_BTN); 
    ImageButton* rect     = new ImageButton(start + MPoint(70, 40), btnSize, rectLogo, window);

    window->registerObject(brush);
    window->registerObject(rect);

    return window;
}

void runMainCycle() {
    RenderTarget renderTarget = RenderTarget(MPoint(0, 0), MPoint(1920, 1080), true);

    Brush* defaultTool        = new Brush();
    ToolManager manager       = ToolManager(defaultTool, MColor::RED);
    FilterManager filtManager = FilterManager();
    WindowManager winManager  = WindowManager();

    Widget drawWidget  = Widget(MPoint(0, 0), MPoint(1920, 1080), nullptr);
    Window* mainWindow = new Window(MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT), MPoint(1900, 1060), MAIN_WINDOW_NAME, &manager, &filtManager, &winManager, false, &drawWidget);
    drawWidget.registerObject(new Rectangle(MPoint(0, 0), MPoint(1920, 1080), MColor::BLACK, MColor::TRANSPARENT, &drawWidget));
    drawWidget.registerObject(mainWindow);

    //create graphics picker of tools and colors
    // Window* pickerWindow = createPickerWindow(mainWindow, &manager, &filtManager);
    // mainWindow->registerObject(pickerWindow);

    // create event manager
    EventManager eventBoy = EventManager();
    eventBoy.registerObject(mainWindow);

    // create bar with tool picker, color picker, and new window creator
    List<SubMenuArgs*> toolArgs; List<ColPickerArgs*> colArgs; List<ModalWindowArgs*> modArgs;
    Menu* actions = createActionMenu(&drawWidget, mainWindow, &manager, &filtManager, &eventBoy, &winManager, toolArgs, colArgs, modArgs);
    mainWindow->setActions(actions);

    renderTarget.clearAll();
    drawWidget.render(&renderTarget);
    renderTarget.displayAll();

    runEventCycle(renderTarget, eventBoy, drawWidget);

    for (size_t i = 0; i < toolArgs.getSize(); i++) delete toolArgs[i];
    for (size_t i = 0; i < colArgs .getSize(); i++) delete colArgs [i];
    for (size_t i = 0; i < modArgs .getSize(); i++) delete modArgs [i];
}

int main() {
    runMainCycle();

    return 0;
}