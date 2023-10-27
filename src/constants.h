#ifndef _CONSTANTS_h_
#define _CONSTANTS_h_

#include <SFML/Graphics.hpp>

static const char*     DEFAULT_FONT       = "assets/MS_SANS.ttf";
static const char*     CLOSE_BTN          = "assets/close_btn.png";
static const char*     RESTORE_BTN        = "assets/restore.png";
static const char*     MINIMIZE_BTN       = "assets/minimize.png";

static const char*     MAIN_WINDOW_NAME   = "Photoshop";
static const int       ACTION_BTN_LEN     = 100;
static const int       ACTION_BTN_HEIGHT  = 30;

static const int       LINE_DIAM          = 1;
static const int       SUB_MENU_LEN       = 300;
static const int       BTN_TXT_PT         = 25;

static const int       OUTLINE_THICKNESS  = 2;
static const int       MAIN_WIN_BRD_SHIFT = 100;
static const sf::Color DEFAULT_BACK_COL   = sf::Color::White;
static const int       TOP_PANE_SIZE      = 30;
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