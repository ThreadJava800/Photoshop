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

static const char* PLUGINS[] = {
    // "/home/vladimir/Projects/Photoshop/plugins/Lol.so",
    // "/home/vladimir/Projects/Photoshop/plugins/monochrome.so",
    // "/home/vladimir/Projects/Photoshop/plugins/monoParam.so",
    // "/home/vladimir/Projects/Photoshop/plugins/plug1.so",
    // "/home/vladimir/Projects/Photoshop/plugins/libsphere_filter.so",
    // "/home/vladimir/Projects/Photoshop/plugins/libconst_fill_plugin.so",
    // "/home/vladimir/Projects/Photoshop/plugins/Brush.so",
    // "/home/vladimir/Projects/Photoshop/plugins/balloon.so",
    // "/home/vladimir/Projects/Photoshop/plugins/curves.so",
    // "/home/vladimir/Projects/Photoshop/plugins/SymCurve.so"
    // "/home/vladimir/Projects/Photoshop/plugins/monochrome.so",
    // "/home/vladimir/Projects/Photoshop/plugins/monoParam.so",
    "/home/vladimir/Projects/Photoshop/plugins/ver2/curves.so",
    "/home/vladimir/Projects/Photoshop/plugins/ver2/balloon.so",
    "/home/vladimir/Projects/Photoshop/plugins/ver2/monochrome.so",
    "/home/vladimir/Projects/Photoshop/plugins/ver2/monoParam.so",
    "/home/vladimir/Projects/Photoshop/plugins/ver2/bwplug.so",
    "/home/vladimir/Projects/Photoshop/plugins/ver2/rect_tool.so",
    "/home/vladimir/Projects/Photoshop/plugins/ver2/spline_tool.so",
    "/home/vladimir/Projects/Photoshop/plugins/ver2/ars_curves.so"
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
    plugin::Plugin   *   plugin_instance = nullptr;

    double saturCoeff;
    MColor color;

    List<ModalWindowArgs*>* modArgs;
    MPoint                  subMenuStart;

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

    if (modalWinArgs->plugin_instance 
     && modalWinArgs->plugin_instance->type == plugin::InterfaceType::System) 
        modalWinArgs->plugin_instance->selectPlugin();

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
    canv_win->getCanvas()->setTexture(draw_img);

    delete draw_img;
}

void openFile(void* arg) {
    if (!arg) return;

    ModalWindowArgs*   modalWinArgs = (ModalWindowArgs*) arg;

    const char* param_names_arr[1] = {"Enter filename"};
    plugin::Array<const char*> paramNames(1, param_names_arr);
    EditBoxModal* modalWindow  = new EditBoxModal(modalWinArgs->evManager, MPoint(300, 300), MPoint(500, 500), "Choose filename", nullptr, modalWinArgs->filtManager, modalWinArgs->drawZone, paramNames);
    
    modalWinArgs->editBoxModal = modalWindow;
    
    modalWindow->setOnDestroy(createCanvasFromTxt);
    modalWindow->setDestrArgs(modalWinArgs);

    EditBox* editBox = new EditBox(MPoint(300, 400), MPoint(300, 50), modalWindow, new MFont(DEFAULT_FONT), ALL_CHARACTER, paramNames.data[0]);

    modalWindow->addEditBox(editBox);
    modalWinArgs->drawZone->registerObject(modalWindow);

    modalWinArgs->subMenu->changeActivity();
}

void saveCanvas(void* arg) {
    if (!arg) return;

    ModalWindowArgs* modWinArgs = (ModalWindowArgs*) arg;
    EditBoxModal   * modWindow  = modWinArgs->editBoxModal;

    List<EditBox*>* editBoxes = modWindow->getEditBoxes();
    char*     fileName  = (*editBoxes)[0]->getText();

    plugin::Texture* plugin_texture = modWinArgs->curWindow->getCanvas()->getTexture();
    MImage texture = MImage(plugin_texture);
    bool res = texture.saveToFile(fileName);

    delete plugin_texture;
    if (!res) return;

    modWinArgs->curWindow->setName(fileName);
}

void saveBtnFunc(void* arg) {
    if (!arg) return;

    ModalWindowArgs*   modalWinArgs = (ModalWindowArgs*) arg;

    const char* param_names_arr[1] = {"Enter filename"};
    plugin::Array<const char*> paramNames(1, param_names_arr);
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

void priorWindow(void* args) {
    ModalWindowArgs* to_prior = (ModalWindowArgs*) args;
    to_prior->curWindow->prioritizeWindow();
    to_prior->subMenu->changeActivity();
}

void openWindowsMenu(void* arg) {
    if (!arg) return;

    ModalWindowArgs* mod_win_arg = (ModalWindowArgs*) arg;

    WindowManager* winManager = mod_win_arg->winManager;
    size_t         winCnt     = winManager->getCanvasWindows()->getSize();

    for (size_t i = 0; i < winCnt; i++) {
        const char* winName = (*(winManager)->getCanvasWindows())[i]->getName();

        ModalWindowArgs* fileBtnArgs = new ModalWindowArgs();
        fileBtnArgs->curWindow    = (*(winManager)->getCanvasWindows())[i];
        fileBtnArgs->subMenu = mod_win_arg->subMenu;
        mod_win_arg->modArgs->pushBack(fileBtnArgs);
        
        TextButton* fileBtn = new TextButton(mod_win_arg->subMenuStart + MPoint(0, i * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT), DEFAULT_BACK_COL, new MFont(DEFAULT_FONT), winName, mod_win_arg->subMenu, priorWindow, fileBtnArgs);
        mod_win_arg->subMenu->registerObject(fileBtn);
    }

    mod_win_arg->subMenu->changeActivity();
}

void openSaveMenu(void* arg) {
    if (!arg) return;

    ModalWindowArgs* mod_win_arg = (ModalWindowArgs*) arg;

    WindowManager* winManager = mod_win_arg->winManager;
    size_t         winCnt     = winManager->getCanvasWindows()->getSize();

    for (size_t i = 0; i < winCnt; i++) {
        const char* winName = (*(winManager)->getCanvasWindows())[i]->getName();

        ModalWindowArgs* fileBtnArgs = new ModalWindowArgs(mod_win_arg->drawZone, mod_win_arg->subMenu, mod_win_arg->evManager, mod_win_arg->filtManager, mod_win_arg->winManager);
        fileBtnArgs->curWindow    = (*(winManager)->getCanvasWindows())[i];
        fileBtnArgs->subMenuChild = mod_win_arg->subMenuChild;
        mod_win_arg->modArgs->pushBack(fileBtnArgs);
        
        TextButton* fileBtn = new TextButton(mod_win_arg->subMenuStart + MPoint(0, i * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT), DEFAULT_BACK_COL, new MFont(DEFAULT_FONT), winName, mod_win_arg->subMenuChild, saveBtnFunc, fileBtnArgs);
        mod_win_arg->subMenuChild->registerObject(fileBtn);
    }

    mod_win_arg->subMenuChild->changeActivity();
}

void chooseTool(void* arg) {
    if (!arg) return;
    ModalWindowArgs* modWinArgs = (ModalWindowArgs*) arg;

    // delete modWinArgs->toolManager->getTool();

    if (modWinArgs->plugin_instance) modWinArgs->plugin_instance->selectPlugin();
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

SubMenu* createToolPicker(ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs, ChooseToolWindow* tool_win) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    SubMenu* toolMenu = new SubMenu(start + MPoint(ACTION_BTN_LEN    , 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, 12 * TOP_PANE_SIZE), arg.drawZone);

    plugin::ToolI*   brush_tool  = new Brush();
    ModalWindowArgs* brushArgs   = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, brush_tool);
    TextButton* brushBtn         = new TextButton(start + MPoint(ACTION_BTN_LEN, 2 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Brush", toolMenu, chooseTool, brushArgs);
    tool_win->addTool(brush_tool);
  
    plugin::ToolI*   line_tool   = new LineTool();
    ModalWindowArgs* lineArgs    = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, line_tool);
    TextButton* lineBtn          = new TextButton(start + MPoint(ACTION_BTN_LEN, 3 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Line", toolMenu, chooseTool, lineArgs);
    tool_win->addTool(line_tool);

    plugin::ToolI*   square_tool = new SquareTool();
    ModalWindowArgs* rectArgs    = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, square_tool);
    TextButton* rectBtn          = new TextButton(start + MPoint(ACTION_BTN_LEN, 4 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Rectangle", toolMenu, chooseTool, rectArgs);
    tool_win->addTool(square_tool);

    plugin::ToolI*   ellipse_tool = new EllipseTool();
    ModalWindowArgs* ellipseArgs  = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, ellipse_tool);
    TextButton* ellipseBtn        = new TextButton(start + MPoint(ACTION_BTN_LEN, 5 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Ellipse", toolMenu, chooseTool, ellipseArgs);
    tool_win->addTool(ellipse_tool);

    plugin::ToolI*   fill_tool   = new FillTool();
    ModalWindowArgs* fillArgs    = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, fill_tool);
    TextButton* fillBtn          = new TextButton(start + MPoint(ACTION_BTN_LEN, 8 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Fill", toolMenu, chooseTool, fillArgs);
    tool_win->addTool(fill_tool);

    plugin::ToolI*   curve_tool  = new CurveTool();
    ModalWindowArgs* curveArgs   = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, curve_tool);
    TextButton* curveBtn         = new TextButton(start + MPoint(ACTION_BTN_LEN, 6 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Curve", toolMenu, chooseTool, curveArgs);
    tool_win->addTool(curve_tool);

    plugin::ToolI*   spline_tool = new Spline();
    ModalWindowArgs* splineArgs  = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, spline_tool);
    TextButton* splineBtn        = new TextButton(start + MPoint(ACTION_BTN_LEN, 7 * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Spline", toolMenu, chooseTool, splineArgs);
    tool_win->addTool(spline_tool);

    modArgs.pushBack(brushArgs);
    modArgs.pushBack(lineArgs);
    modArgs.pushBack(rectArgs);
    modArgs.pushBack(ellipseArgs);
    modArgs.pushBack(curveArgs);
    modArgs.pushBack(splineArgs);
    modArgs.pushBack(fillArgs);

    toolMenu->registerObject(brushBtn);
    toolMenu->registerObject(lineBtn);
    toolMenu->registerObject(rectBtn);
    toolMenu->registerObject(ellipseBtn);
    toolMenu->registerObject(curveBtn);
    toolMenu->registerObject(splineBtn);
    toolMenu->registerObject(fillBtn);

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

void loadPlugins(ChooseToolWindow* tool_win, SubMenu* filtMenu, SubMenu* toolMenu, ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs, plugin::App* _app, MPoint start_ind, MPoint start, MPoint size, MColor color) {
    int tool_cnt = 0, filter_cnt = 0;

    for (int i = 0; i < sizeof(PLUGINS) / sizeof(const char*); i++) {

        void* filt_lib            = dlopen(PLUGINS[i], RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);
        // fprintf(stderr, "OPEN: %s\n", dlerror());
        getInstFunc get_inst_func = (getInstFunc)(dlsym(filt_lib, "getInstance"));
        // fprintf(stderr, "OPEN: %s\n", dlerror());
        plugin::Plugin* plugin    = get_inst_func(_app);
        // fprintf(stderr, "OPEN: %s\n", dlerror());
        dlclose(filt_lib);

        plugins.pushBack(plugin);

        if (plugin->type == plugin::InterfaceType::Filter) {
            ModalWindowArgs* modWinArg = new ModalWindowArgs(arg.drawZone, filtMenu, arg.evManager, arg.filtManager, nullptr, nullptr, plugin->getInterface());
            modWinArg->plugin_instance = plugin;

            TextButton* text_label = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, (start_ind.y + filter_cnt) * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), plugin->name, filtMenu, addPluginFilter, modWinArg);
            filtMenu->registerObject(text_label);

            modArgs.pushBack(modWinArg);

            filter_cnt++;
            continue;
        }

        if (plugin->type == plugin::InterfaceType::Tool) {
            ModalWindowArgs* modWinArg2 = new ModalWindowArgs(nullptr, toolMenu, nullptr, arg.filtManager, nullptr, arg.toolManager, plugin->getInterface());
            modWinArg2->plugin_instance = plugin;
            
            tool_win->addTool((plugin::ToolI*)plugin->getInterface());

            TextButton* text_label = new TextButton(start + MPoint(ACTION_BTN_LEN, (start_ind.x + tool_cnt) * TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), plugin->name, toolMenu, chooseTool, modWinArg2);
            toolMenu->registerObject(text_label);

            modArgs.pushBack(modWinArg2);
            tool_cnt++;
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

    ModalWindowArgs* saveArgs = new ModalWindowArgs(arg.drawZone, fileMenu, arg.evManager, arg.filtManager, arg.winManager);
    saveArgs->subMenuChild = chooseMenu;
    saveArgs->modArgs      = &modArgs;
    saveArgs->subMenuStart = chooseMenuPos;

    TextButton* saveBtn = new TextButton(start + MPoint(0, 2 * TOP_PANE_SIZE), size, color, new MFont(DEFAULT_FONT), "Save", fileMenu, openSaveMenu, saveArgs);

    ModalWindowArgs* openArg = new ModalWindowArgs(arg.drawZone, fileMenu, arg.evManager, arg.filtManager, arg.winManager, arg.toolManager);
    TextButton*      openBtn = new TextButton     (start + MPoint(0, 3 * TOP_PANE_SIZE), size, color, new MFont(DEFAULT_FONT), "Open", fileMenu, openFile, openArg);

    fileMenu->registerObject(saveBtn);
    fileMenu->registerObject(openBtn);
    fileMenu->registerObject(chooseMenu);

    modArgs.pushBack(modWinArgs);
    modArgs.pushBack(openArg);
    modArgs.pushBack(saveArgs);

    return fileMenu;
}

SubMenu* createMenuWindow(ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    List<Window*>* windows = arg.winManager->getCanvasWindows();
    size_t   winCnt        = windows->getSize();
    SubMenu* menu          = new SubMenu(start + MPoint(ACTION_BTN_LEN * 4, 2 * TOP_PANE_SIZE), MPoint(ACTION_BTN_LEN * 2, winCnt * TOP_PANE_SIZE), arg.drawZone);

    return menu;
}

Menu* createActionMenu(ModalWindowArgs& arg, List<ModalWindowArgs*>& modArgs, plugin::App* _app, ChooseToolWindow* tool_win) {
    MPoint start = MPoint(MAIN_WIN_BRD_SHIFT, MAIN_WIN_BRD_SHIFT);
    MPoint size  = MPoint(ACTION_BTN_LEN, ACTION_BTN_HEIGHT);
    MColor color = MColor(DEFAULT_BACK_COL);

    Menu*    actionMenu = new Menu(start + MPoint(0, TOP_PANE_SIZE), MPoint(4 * ACTION_BTN_LEN, TOP_PANE_SIZE), arg.drawZone);
    SubMenu* fileMenu   = createFileMenu   (arg, modArgs);
    SubMenu* toolMenu   = createToolPicker (arg, modArgs, tool_win);
    SubMenu* colMenu    = createColorPicker(arg, modArgs);
    SubMenu* filtMenu   = createFilterMenu (arg, modArgs, _app);
    SubMenu* menuMenu   = createMenuWindow (arg, modArgs);

    loadPlugins(tool_win, filtMenu, toolMenu, arg, modArgs, _app, MPoint(9, 7), start, size, color);

    TextButton* fileBtn   = new TextButton(start + MPoint(0,                  TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "File",   actionMenu, openToolMenu, fileMenu);
    TextButton* toolBtn   = new TextButton(start + MPoint(ACTION_BTN_LEN,     TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Tools",  actionMenu, openToolMenu, toolMenu);
    TextButton* colBtn    = new TextButton(start + MPoint(ACTION_BTN_LEN * 2, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Color",  actionMenu, openToolMenu, colMenu);
    TextButton* filterBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 3, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Filter", actionMenu, openToolMenu, filtMenu);
    
    ModalWindowArgs* window_args = new ModalWindowArgs();
    window_args->winManager   = arg.winManager;
    window_args->subMenu      = menuMenu;
    window_args->modArgs      = &modArgs;
    window_args->subMenuStart = MPoint(menuMenu->getPos());
    TextButton* windowBtn = new TextButton(start + MPoint(ACTION_BTN_LEN * 4, TOP_PANE_SIZE), size, color, new MFont (DEFAULT_FONT), "Windows", actionMenu, openWindowsMenu, window_args);

    actionMenu->registerObject(fileBtn);
    actionMenu->registerObject(toolBtn);
    actionMenu->registerObject(colBtn);
    actionMenu->registerObject(filterBtn);
    actionMenu->registerObject(windowBtn);

    actionMenu->registerObject(fileMenu);
    actionMenu->registerObject(toolMenu);
    actionMenu->registerObject(colMenu);
    actionMenu->registerObject(filtMenu);
    actionMenu->registerObject(menuMenu);

    return actionMenu;
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

    MGUI gui_i = MGUI(&eventBoy, drawWidget.getSize(), mainWindow);
    plugin::App app_instance;
    app_instance.root           = &gui_i;
    app_instance.event_manager  = &eventBoy;

    ChooseToolWindow* tool_win = new ChooseToolWindow(MPoint(1920 - 2 * PICKER_EDGE_SHIFT - 2 * PICKER_BTN_SIZE - 3* PICKER_EDGE_SHIFT, 100), MPoint(3 * PICKER_EDGE_SHIFT + 2 * PICKER_BTN_SIZE, 700), mainWindow, &filtManager, &manager);
    mainWindow->registerSubWidget(tool_win);

    // create bar with tool picker, color picker, and new window creator
    List<ModalWindowArgs*> modArgs;
    ModalWindowArgs window_arg = ModalWindowArgs(&drawWidget, nullptr, &eventBoy, &filtManager, &winManager, &manager);
    Menu* actions = createActionMenu(window_arg, modArgs, &app_instance, tool_win);
    mainWindow->setActions(actions);

    renderTarget.clearAll();
    drawWidget.render(&renderTarget);
    renderTarget.displayAll();

    runEventCycle(renderTarget, eventBoy, drawWidget);

    for (size_t i = 0; i < modArgs.getSize(); i++) {
        delete modArgs[i]->plugin;
        delete modArgs[i];
    }
}

int main() {
    runMainCycle();

    return 0;
}