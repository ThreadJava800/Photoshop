#ifndef _CONSTANTS_h_
#define _CONSTANTS_h_

#include <SFML/Graphics.hpp>

static const char*     DEFAULT_FONT       = "../assets/MS_SANS.ttf";
static const int       MAIN_WIN_BRD_SHIFT = 100;
static const sf::Color DEFAULT_BACK_COL   = sf::Color::White;

#define ON_ERROR(expr, errStr, retVal) {                                                   \
    if (expr) {                                                                             \
        fprintf(stderr, "Error(%s) occured in " __FILE__ " in line %d", errStr, __LINE__);   \
        return retVal;                                                                        \
    }                                                                                          \
}                                                                                               \


#endif