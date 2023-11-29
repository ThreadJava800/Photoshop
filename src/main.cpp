#include <chrono>
#include <dlfcn.h>

#include "includes.h"
#include "ui/window/window.h"
#include "ui/submenu/submenu.h"
#include "events/events.h"
#include "ui/editbox/editbox.h"
#include "../libs/multimedia/eventInterlayer.h"
#include "ui/shapes/shapes.h"
#include "pluginDriver.h"

List<plugin::Plugin*> plugins;

// TODO:
// p^2, phi + rand(0, 1)^2 => числа будут тяготеть к границу (из плотности вероятности возникновения корня)

static const char* PLUGINS[] = {
    "/home/vladimir/Projects/Photoshop/plugins/Lol.so",
    "/home/vladimir/Projects/Photoshop/plugins/monochrome.so",
    "/home/vladimir/Projects/Photoshop/plugins/monoParam.so",
    "/home/vladimir/Projects/Photoshop/plugins/plug1.so",
    "/home/vladimir/Projects/Photoshop/plugins/libsphere_filter.so",
    "/home/vladimir/Projects/Photoshop/plugins/libfill_tool_plugin.so",
    "/home/vladimir/Projects/Photoshop/plugins/Brush.so",
    "/home/vladimir/Projects/Photoshop/plugins/balloon.so"
};

typedef plugin::Plugin* (*getInstFunc)(plugin::App*);

struct ModalWindowArgs {
    Widget *         drawZone;
    SubMenu*         subMenu;
    SubMenu*         subMenuChild;

    EventManager *   evManager;
    FilterManager*   filtManager;
    ToolManager  *   toolManager;
    WindowManager*   winManager;

    Window           *   curWindow;
    EditBoxModal     *   editBoxModal;
    plugin::Interface*   plugin;

    double saturCoeff;
    MColor color;

    ModalWindowArgs() {};

    ModalWindowArgs(Widget* _drawZone, SubMenu* _subMenu, EventManager* _evManager, FilterManager* _filtManager, WindowManager* _winManager, ToolManager* _toolManager = nullptr, plugin::Interface* _plugin = nullptr, MColor _color = MColor::TRANSPARENT, double _saturCoeff = 1.0) :
        drawZone   (_drawZone),
        subMenu    (_subMenu),
        evManager  (_evManager),
        filtManager(_filtManager),
        winManager (_winManager),
        toolManager(_toolManager),
        plugin     (_plugin),
        color      (_color),
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

    plugin::FilterI*      filter     = modWindow->getFiltManager()->getLast();
    plugin::Array<double> doubleArgs = modWindow->getParams();

    filter->setParams(doubleArgs);
    modWindow->getFiltManager()->setActive(true);
}

void addPluginFilter(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modalWinArgs = (ModalWindowArgs*) arg;

    plugin::FilterI* filter = (plugin::FilterI*)modalWinArgs->plugin;

    plugin::Array params = filter->getParamNames();
    if (params.size > 0) {
        PluginParamWindow* modalWindow = new PluginParamWindow(modalWinArgs->evManager, modalWinArgs->drawZone, params, filter);
        modalWinArgs->drawZone->registerObject(modalWindow);
    }

    modalWinArgs->filtManager->setFilter(filter);
    modalWinArgs->filtManager->setNeedFree(false);
    modalWinArgs->filtManager->setActive(true);

    modalWinArgs->subMenu->changeActivity();
}

void createCanvasFromTxt(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modalWinArgs = (ModalWindowArgs*) arg;
    EditBoxModal   * modWindow    = modalWinArgs->editBoxModal;

    List<EditBox*>* editBoxes = modWindow->getEditBoxes();
    char* fileName = (*editBoxes)[0]->getText();

    MImage           load_img = MImage(fileName);
    plugin::Texture* draw_img = load_img.toPluginTexture();

    char canv_name[128];
    sprintf(canv_name, "Canvas %d", modalWinArgs->winManager->getCanvasWindows()->getSize() + 1);

    Window* main_win = modalWinArgs->winManager->getMainWindow();
    Window* canv_win = new Window(MPoint(300, 200), MPoint(std::min(1200, (int)draw_img->width), std::min(800, (int)draw_img->height)), canv_name, modalWinArgs->toolManager, modalWinArgs->filtManager, modalWinArgs->winManager, true, main_win);

    canv_win->createCanvas(draw_img->width, draw_img->height);
    main_win->registerObject(canv_win);
    modalWinArgs->winManager->getCanvasWindows()->pushBack(canv_win);
    canv_win->getCanvas()->setTexture(draw_img);

    delete[] draw_img->pixels;
    delete   draw_img;
}

void openFile(void* arg) {
    if (!arg) return;

    ModalWindowArgs*   modalWinArgs = (ModalWindowArgs*) arg;

    plugin::Array<const char*> paramNames;
    paramNames.size    = 1;
    paramNames.data    = new const char*[paramNames.size];
    paramNames.data[0] = "Enter filename";
    EditBoxModal* modalWindow  = new EditBoxModal(modalWinArgs->evManager, MPoint(300, 300), MPoint(500, 500), "Choose filename", nullptr, modalWinArgs->filtManager, modalWinArgs->drawZone, paramNames);
    
    modalWinArgs->editBoxModal = modalWindow;
    
    modalWindow->setOnDestroy(createCanvasFromTxt);
    modalWindow->setDestrArgs(modalWinArgs);

    EditBox* editBox = new EditBox(MPoint(300, 400), MPoint(300, 50), modalWindow, new MFont(DEFAULT_FONT), ALL_CHARACTER, paramNames.data[0]);

    modalWindow->addEditBox(editBox);
    modalWinArgs->drawZone->registerObject(modalWindow);

    modalWinArgs->subMenu->changeActivity();
}

void chooseTool(void* arg) {
    if (!arg) return;
    ModalWindowArgs* modWinArgs = (ModalWindowArgs*) arg;

    // delete modWinArgs->toolManager->getTool();

    modWinArgs->toolManager->setTool((plugin::ToolI*)modWinArgs->plugin);
    modWinArgs->subMenu->changeActivity();
    modWinArgs->filtManager->setActive(false);
}

void chooseColor(void* arg) {
    if (!arg) return;
    ModalWindowArgs* modWinArgs = (ModalWindowArgs*) arg;

    modWinArgs->toolManager->setColor(modWinArgs->color.toPlColor());
    modWinArgs->subMenu->changeActivity();
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

    WindowManager* _winManager   = modWinArgs->winManager;
    size_t          winCnt       = modWinArgs->subMenuChild->getWindows()->getSize();
    List<WidgetPtr>* subMenuBtns = modWinArgs->subMenuChild->getWindows();

    for (size_t i = 0; i < winCnt; i++) {
        const char* winName = (*_winManager->getCanvasWindows())[i]->getName();

        WidgetPtr widget_ptr = (*subMenuBtns)[i];
        if (!widget_ptr.is_extern) {
            ((TextButton*)widget_ptr.program_widget)->setText(winName);
        }
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

    EditBox* editBox = new EditBox(MPoint(300, 400), MPoint(300, 50), modalWindow, new MFont(DEFAULT_FONT), ALL_CHARACTER, paramNames.data[0]);

    modalWindow->addEditBox(editBox);
    modalWinArgs->drawZone->registerObject(modalWindow);

    modalWinArgs->subMenuChild->changeActivity();
    modalWinArgs->subMenu     ->changeActivity();
}

SubMenu* createToolPicker(ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* toolMenu = new SubMenu(start + MPoint(ACTION_BTN_LEN    , 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 12 * TOP_PANE_SIZE), arg.drawZone);

    ModalWindowArgs* brushArgs   = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager,new Brush());
    TextButton* brushBtn         = new TextButton(start + MPoint(ACTION_BTN_LEN, 2 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Brush", toolMenu, chooseTool, brushArgs);
  
    ModalWindowArgs* lineArgs    = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, new LineTool());
    TextButton* lineBtn          = new TextButton(start + MPoint(ACTION_BTN_LEN, 3 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Line", toolMenu, chooseTool, lineArgs);
  
    ModalWindowArgs* rectArgs    = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, new SquareTool());
    TextButton* rectBtn          = new TextButton(start + MPoint(ACTION_BTN_LEN, 4 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Rectangle", toolMenu, chooseTool, rectArgs);

    ModalWindowArgs* ellipseArgs = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, new EllipseTool());
    TextButton* ellipseBtn       = new TextButton(start + MPoint(ACTION_BTN_LEN, 5 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Ellipse", toolMenu, chooseTool, ellipseArgs);

    ModalWindowArgs* curveArgs   = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, new CurveTool());
    TextButton* curveBtn         = new TextButton(start + MPoint(ACTION_BTN_LEN, 6 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Curve", toolMenu, chooseTool, curveArgs);

    ModalWindowArgs* splineArgs  = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, new Spline());
    TextButton* splineBtn        = new TextButton(start + MPoint(ACTION_BTN_LEN, 7 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Spline", toolMenu, chooseTool, splineArgs);

    // ModalWindowArgs* fillArgs    = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, new FillTool());
    // TextButton* fillBtn          = new TextButton(start + MPoint(ACTION_BTN_LEN, 8 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Fill", toolMenu, chooseTool, fillArgs);

    modArgs.pushBack(brushArgs);
    modArgs.pushBack(lineArgs);
    modArgs.pushBack(rectArgs);
    modArgs.pushBack(ellipseArgs);
    modArgs.pushBack(curveArgs);
    modArgs.pushBack(splineArgs);
    // modArgs.pushBack(fillArgs);

    toolMenu->registerObject(brushBtn);
    toolMenu->registerObject(lineBtn);
    toolMenu->registerObject(rectBtn);
    toolMenu->registerObject(ellipseBtn);
    toolMenu->registerObject(curveBtn);
    toolMenu->registerObject(splineBtn);
    // toolMenu->registerObject(fillBtn);

    return toolMenu;
}

SubMenu* createColorPicker(ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* colMenu  = new SubMenu(start + MPoint(ACTION_BTN_LEN * 2, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 9 * TOP_PANE_SIZE), arg.drawZone);

    ModalWindowArgs* redArgs     = new ModalWindowArgs(nullptr, colMenu, nullptr, nullptr, nullptr, arg.toolManager, nullptr, MColor::RED);
    TextButton* redBtn           = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 2 * TOP_PANE_SIZE), size, MColor::RED, new MFont (DEFAULT_FONT), "Red", colMenu, chooseColor, redArgs);
    
    ModalWindowArgs* greenArgs   = new ModalWindowArgs(nullptr, colMenu, nullptr, nullptr, nullptr, arg.toolManager, nullptr, MColor::GREEN);
    TextButton* greenBtn         = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 3 * TOP_PANE_SIZE), size, MColor::GREEN, new MFont (DEFAULT_FONT), "Green", colMenu, chooseColor, greenArgs);
    
    ModalWindowArgs* yellowArgs  = new ModalWindowArgs(nullptr, colMenu, nullptr, nullptr, nullptr, arg.toolManager, nullptr, MColor::YELLOW);
    TextButton* yellowBtn        = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 4 * TOP_PANE_SIZE), size, MColor::YELLOW, new MFont (DEFAULT_FONT), "Yellow", colMenu, chooseColor, yellowArgs);
    
    ModalWindowArgs* blueArgs    = new ModalWindowArgs(nullptr, colMenu, nullptr, nullptr, nullptr, arg.toolManager, nullptr, MColor::BLUE);
    TextButton* blueBtn          = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 5 * TOP_PANE_SIZE), size, MColor::BLUE, new MFont (DEFAULT_FONT), "Blue", colMenu, chooseColor, blueArgs);
    
    ModalWindowArgs* blackArgs   = new ModalWindowArgs(nullptr, colMenu, nullptr, nullptr, nullptr, arg.toolManager, nullptr, MColor::BLACK);
    TextButton* blackBtn         = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 6 * TOP_PANE_SIZE), size, MColor::BLACK, new MFont (DEFAULT_FONT), "Black", colMenu, chooseColor, blackArgs);
    
    ModalWindowArgs* cyanArgs    = new ModalWindowArgs(nullptr, colMenu, nullptr, nullptr, nullptr, arg.toolManager, nullptr, MColor::CYAN);
    TextButton* cyanBtn          = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 7 * TOP_PANE_SIZE), size, MColor::CYAN, new MFont (DEFAULT_FONT), "Cyan", colMenu, chooseColor, cyanArgs);
    
    ModalWindowArgs* magentaArgs = new ModalWindowArgs(nullptr, colMenu, nullptr, nullptr, nullptr, arg.toolManager, nullptr, MColor::MAGENTA);
    TextButton*    magentaBtn    = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, 8 * TOP_PANE_SIZE), size, MColor::MAGENTA, new MFont (DEFAULT_FONT), "Magenta", colMenu, chooseColor, magentaArgs);

    modArgs.pushBack(redArgs);
    modArgs.pushBack(greenArgs);
    modArgs.pushBack(yellowArgs);
    modArgs.pushBack(blueArgs);
    modArgs.pushBack(blackArgs);
    modArgs.pushBack(cyanArgs);
    modArgs.pushBack(magentaArgs);

    colMenu->registerObject(redBtn);
    colMenu->registerObject(greenBtn);
    colMenu->registerObject(yellowBtn);
    colMenu->registerObject(blueBtn);
    colMenu->registerObject(blackBtn);
    colMenu->registerObject(cyanBtn);
    colMenu->registerObject(magentaBtn);

    return colMenu;
}
#include <assert.h>
void loadPlugins(SubMenu* filtMenu, SubMenu* toolMenu, ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs, plugin::App* _app, MPoint start_ind, MPoint start, MPoint size, MColor color) {
    for (int i = 0; i < sizeof(PLUGINS) / sizeof(const char*); i++) {

        void* filt_lib            = dlopen(PLUGINS[i], RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);
        getInstFunc get_inst_func = (getInstFunc)(dlsym(filt_lib, "getInstance"));
        plugin::Plugin* plugin    = get_inst_func(_app);
        dlclose(filt_lib);

        plugins.pushBack(plugin);

        if (plugin->type == plugin::InterfaceType::Filter) {
            ModalWindowArgs* modWinArg = new ModalWindowArgs(arg.drawZone, filtMenu, arg.evManager, arg.filtManager, nullptr, nullptr, plugin->getInterface());

            TextButton* text_label = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, (start_ind.x + i) * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), plugin->name, filtMenu, addPluginFilter, modWinArg);
            filtMenu->registerObject(text_label);

            modArgs.pushBack(modWinArg);
            continue;
        }

        if (plugin->type == plugin::InterfaceType::Tool) {
            std::cerr << PLUGINS[i] << ' ' << plugin->getInterface() << '\n';
            ModalWindowArgs* modWinArg2 = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, plugin->getInterface());
            TextButton* text_label = new TextButton(start + MPoint(ACTION_BTN_LEN, (start_ind.y + i) * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), plugin->name, toolMenu, chooseTool, modWinArg2);
            toolMenu->registerObject(text_label);

            modArgs.pushBack(modWinArg2);
        }
    }
}

SubMenu* createFilterMenu(ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs, plugin::App* _app) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* filtMenu = new SubMenu(start + MPoint(ACTION_BTN_LEN * 3, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 12 * TOP_PANE_SIZE), arg.drawZone);

    ModalWindowArgs* lastArgs = new ModalWindowArgs(arg.drawZone, filtMenu, arg.evManager, arg.filtManager, nullptr, nullptr, arg.toolManager->getTool());
    TextButton*      lastBtn  = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 2 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Last used", filtMenu, addPluginFilter, lastArgs);
    
    ModalWindowArgs* constBrightArg = new ModalWindowArgs(arg.drawZone, filtMenu, arg.evManager, arg.filtManager, nullptr, nullptr, new BrightnessFilter(false));
    TextButton*      constBrightBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 3 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Blur (const)", filtMenu, addPluginFilter, constBrightArg);

    ModalWindowArgs* customBlurArg = new ModalWindowArgs(arg.drawZone, filtMenu, arg.evManager, arg.filtManager, nullptr, nullptr, new BrightnessFilter(true));
    TextButton*      customBlurBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 4 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Blur (custom)", filtMenu, addPluginFilter, customBlurArg);

    ModalWindowArgs* colorfulnessUPArg = new ModalWindowArgs(arg.drawZone, filtMenu, arg.evManager, arg.filtManager, nullptr, nullptr, new ColorfulnessFilter(ColorfulnessFilter::SATUR_UP));
    TextButton*      colorfulnessUPBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 5 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Satur (UP)", filtMenu, addPluginFilter, colorfulnessUPArg);

    ModalWindowArgs* colorfulnessDOArg = new ModalWindowArgs(arg.drawZone, filtMenu, arg.evManager, arg.filtManager, nullptr, nullptr, new ColorfulnessFilter(ColorfulnessFilter::SATUR_DOWN));
    TextButton*      colorfulnessDOBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, 6 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Satur (DOWN)", filtMenu, addPluginFilter, colorfulnessDOArg);

    modArgs.pushBack(lastArgs);
    modArgs.pushBack(constBrightArg);
    modArgs.pushBack(customBlurArg);
    modArgs.pushBack(colorfulnessUPArg);
    modArgs.pushBack(colorfulnessDOArg);

    filtMenu->registerObject(lastBtn);
    filtMenu->registerObject(constBrightBtn);
    filtMenu->registerObject(customBlurBtn);
    filtMenu->registerObject(colorfulnessUPBtn);
    filtMenu->registerObject(colorfulnessDOBtn);

    return filtMenu;
}

SubMenu* createFileMenu(ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* fileMenu   = new SubMenu(start + MPoint(0, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 3 * TOP_PANE_SIZE), arg.drawZone);
    size_t   winCnt     = arg.winManager->getCanvasWindows()->getSize();
    ModalWindowArgs* modWinArgs = new ModalWindowArgs(arg.drawZone, fileMenu, arg.evManager, arg.filtManager, arg.winManager);

    MPoint chooseMenuPos = MPoint(fileMenu->getPos()) + MPoint(fileMenu->getSize().x, 0);
    SubMenu* chooseMenu  = new SubMenu(chooseMenuPos, MPoint(fileMenu->getSize().x, TOP_PANE_SIZE * winCnt), fileMenu);
    
    for (size_t i = 0; i < winCnt; i++) {
        const char* winName = (*(arg.winManager)->getCanvasWindows())[i]->getName();

        ModalWindowArgs* fileBtnArgs = new ModalWindowArgs(arg.drawZone, fileMenu, arg.evManager, arg.filtManager, arg.winManager);
        fileBtnArgs->curWindow    = (*(arg.winManager)->getCanvasWindows())[i];
        fileBtnArgs->subMenuChild = chooseMenu;
        modArgs.pushBack(fileBtnArgs);
        
        TextButton* fileBtn = new TextButton(chooseMenuPos + MPoint(0, i * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT), DEFAULT_BACK_COL, new MFont(DEFAULT_FONT), winName, chooseMenu, saveBtnFunc, fileBtnArgs);
        chooseMenu->registerObject(fileBtn);
    }

    TextButton* saveBtn = new TextButton(start + MPoint(0, 2 * TOP_PANE_SIZE), size, color, new MFont(DEFAULT_FONT), "Save", fileMenu, openToolMenu, chooseMenu);

    ModalWindowArgs* openArg = new ModalWindowArgs(arg.drawZone, fileMenu, arg.evManager, arg.filtManager, arg.winManager, arg.toolManager);
    TextButton*      openBtn = new TextButton     (start + MPoint(0, 3 * TOP_PANE_SIZE), size, color, new MFont(DEFAULT_FONT), "Open", fileMenu, openFile, openArg);

    fileMenu->registerObject(saveBtn);
    fileMenu->registerObject(openBtn);
    fileMenu->registerObject(chooseMenu);

    modArgs.pushBack(modWinArgs);
    modArgs.pushBack(openArg);

    return fileMenu;
}

Menu* createActionMenu(ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs, plugin::App* _app) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    Menu*    actionMenu = new Menu(start + MPoint(0, TOP_PANE_SIZE), MPoint(4 * ACTION_BTN_LEN, TOP_PANE_SIZE), arg.drawZone);
    SubMenu* fileMenu   = createFileMenu   (arg, modArgs);
    SubMenu* toolMenu   = createToolPicker (arg, modArgs);
    SubMenu* colMenu    = createColorPicker(arg, modArgs);
    SubMenu* filtMenu   = createFilterMenu (arg, modArgs, _app);

    loadPlugins(filtMenu, toolMenu, arg, modArgs, _app, MPoint(7, 4), start, size, color);

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

    winManager.setMainWindow(mainWindow);

    // //create graphics picker of tools and colors
    // // Window* pickerWindow = createPickerWindow(mainWindow, &manager, &filtManager);
    // // mainWindow->registerObject(pickerWindow);

    // create event manager
    EventManager eventBoy = EventManager();
    eventBoy.registerObject(mainWindow);

    MGUI gui_i = MGUI(&eventBoy, drawWidget.getSize(), &drawWidget);
    plugin::App app_instance;
    app_instance.root           = &gui_i;
    app_instance.event_manager  = &eventBoy;
    app_instance.filter_manager = &filtManager;
    app_instance.tool_manager   = &manager;

    // create bar with tool picker, color picker, and new window creator
    List<ModalWindowArgs*> modArgs;
    ModalWindowArgs window_arg = ModalWindowArgs(&drawWidget, nullptr, &eventBoy, &filtManager, &winManager, &manager);
    Menu* actions = createActionMenu(window_arg, modArgs, &app_instance);
    mainWindow->setActions(actions);

    renderTarget.clearAll();
    drawWidget.render(&renderTarget);
    renderTarget.displayAll();

    runEventCycle(renderTarget, eventBoy, drawWidget);

    for (size_t i = 0; i < modArgs.getSize(); i++) delete modArgs [i];
    for (size_t i = 0; i < plugins.getSize(); i++) delete plugins [i];
}

int main() {
    runMainCycle();

    return 0;
}