#ifndef _CONSTANTS_h_
#define _CONSTANTS_h_

#include <SFML/Graphics.hpp>

typedef void (*ButtonFunc)(void*);

static const char*     DEFAULT_FONT       = "assets/DraftingMono-Bold.ttf";
static const char*     CLOSE_BTN          = "assets/close_btn.png";
static const char*     RESTORE_BTN        = "assets/restore.png";
static const char*     MINIMIZE_BTN       = "assets/minimize.png";
static const char*     BRUSH_BTN          = "assets/brush_logo.png";
static const char*     RECT_BTN           = "assets/rect_tool.png";
static const char*     LEFT_ARROW         = "assets/left.png";
static const char*     RIGHT_ARROW        = "assets/right.png";
static const char*     DOWN_ARROW         = "assets/down.png";
static const char*     UP_ARROW           = "assets/up.png";

static const char*     MAIN_WINDOW_NAME   = "Photoshop";
static const int       ACTION_BTN_LEN     = 100;
static const int       ACTION_BTN_HEIGHT  = 30;
static const int       PICKER_BTN_SIZE    = 50;

static const int       LINE_DIAM          = 1;
static const int       SUB_MENU_LEN       = 300;
static const int       BTN_TXT_PT         = 23;

static const int       CANVAS_SIZE        = 1000;
static const int       CANV_SHIFT         = 5;
static const double    SATURATION_SHIFT   = 0.05;

static const int       OUTLINE_THICKNESS  = 2;
static const int       MAIN_WIN_BRD_SHIFT = 100;
static const sf::Color DEFAULT_BACK_COL   = sf::Color::White;
static const int       TOP_PANE_SIZE      = 30;
static const int       SCROLLBAR_BTN_H    = 10;
static const sf::Color DEFAULT_COLOR      = sf::Color::Transparent;
static const sf::Color BLACK              = sf::Color::Black;
static const sf::Color GRAY               = sf::Color(128, 128, 128);
static const sf::Color TRANSPARENT        = sf::Color::Transparent;
static const sf::Color YELLOW             = sf::Color::Yellow;

static const double    CATMULL_ALPHA      = 0.5;
static const int       BRIGHTNESS_SHIFT   = 10;

static const int DEB_COLS_CNT = 7;
static const sf::Color DEB_COLS[] = {
    sf::Color::Red,
    sf::Color::Yellow,
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Cyan,
    sf::Color::Magenta,
    sf::Color::White
};

#ifndef ON_ERROR
#define ON_ERROR(expr, errStr, retVal) {                                                   \
    if (expr) {                                                                             \
        fprintf(stderr, "Error(%s) occured in " __FILE__ " in line %d", errStr, __LINE__);   \
        return retVal;                                                                        \
    }                                                                                          \
}                                                                                               \

#endif


#endif