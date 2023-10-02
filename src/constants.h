#ifndef _CONSTANTS_h_
#define _CONSTANTS_h_

#include <SFML/Graphics.hpp>

static const char*     DEFAULT_FONT       = "assets/MS_SANS.ttf";
static const char*     CLOSE_BTN          = "assets/close_btn.png";
static const char*     RESTORE_BTN        = "assets/restore.png";
static const char*     MINIMIZE_BTN       = "assets/minimize.png";

static const char*     MAIN_WINDOW_NAME   = "Photoshop";
static const int       ACTION_BTN_LEN     = 80;
static const int       ACTION_BTN_HEIGHT  = 30;

static const int       MAIN_WIN_BRD_SHIFT = 100;
static const sf::Color DEFAULT_BACK_COL   = sf::Color::White;
static const int       TOP_PANE_SIZE      = 30;
static const sf::Color DEFAULT_COLOR      = sf::Color::Red;

#define ON_ERROR(expr, errStr, retVal) {                                                   \
    if (expr) {                                                                             \
        fprintf(stderr, "Error(%s) occured in " __FILE__ " in line %d", errStr, __LINE__);   \
        return retVal;                                                                        \
    }                                                                                          \
}                                                                                               \


#endif