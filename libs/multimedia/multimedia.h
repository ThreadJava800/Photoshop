#ifndef _SUPPORT_h_
#define _SUPPORT_h_

#include "../../src/includes.h"

struct MPoint {
    double x = 0;
    double y = 0;

    explicit MPoint(double _x, double _y);
    explicit MPoint(sf::Vector2f _point);

    ~MPoint();

    sf::Vector2f toSfVector();
};

struct MColor {
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

    explicit MColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a);
    explicit MColor(sf::Color _color);

    ~MColor();

    sf::Color toSfColor();
}; 

struct MFont {
private:
    sf::Font* font = nullptr;

public:
    const char* fontFile = nullptr;

    explicit MFont(const char* _fontFile);

    ~MFont();

    sf::Font* getSfFont();
};

enum MKeyboard {
    ESCAPE = sf::Keyboard::Escape
};

enum MMouse {
    LEFT  = sf::Mouse::Left,
    RIGHT = sf::Mouse::Right
};

class RenderTarget {
private:
    sf::RenderTexture* sprite  = nullptr;
    sf::RenderTexture* texture = nullptr;

public:
    explicit RenderTarget(sf::RenderTexture* _sprite, sf::RenderTexture* _texture);
    ~RenderTarget();

    void drawLine  (MPoint start,  MPoint end,    MColor color);
    void drawRect  (MPoint start,  MPoint size,   MColor color);
    void drawCircle(MPoint centre, double radius, MColor color);
    // void drawSprite(); 
    void drawText  (MPoint start,  const char* text, MColor color, MFont font, unsigned pt);
    void setPixel  (MPoint pos, MColor color);
};

#endif