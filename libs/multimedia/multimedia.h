#ifndef _SUPPORT_h_
#define _SUPPORT_h_

#include "../../src/includes.h"

struct MPoint {
    double x = 0;
    double y = 0;

    explicit MPoint();
    explicit MPoint(double _x, double _y);
    explicit MPoint(sf::Vector2f _point);
    explicit MPoint(sf::Vector2i _point);
    explicit MPoint(sf::Vector2u _point);

    ~MPoint();

    sf::Vector2f toSfVector();

    friend void   operator+=(      MPoint& a, const MPoint& b);
    friend MPoint operator+ (const MPoint& a, const MPoint& b);
    friend void   operator-=(      MPoint& a, const MPoint& b);
    friend MPoint operator- (const MPoint& a, const MPoint& b);
    friend void   operator*=(      MPoint& a, const double  b);
    friend MPoint operator* (const MPoint& a, const double  b);
};

struct MColor {
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

    explicit MColor();
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

    explicit MFont();
    explicit MFont(const char* _fontFile);

    ~MFont();

    sf::Font* getSfFont();
};

struct MImage {
private:
    sf::Texture* img = nullptr;

public:
    const char* imgPath = nullptr;

    explicit MImage();
    explicit MImage(const char* _imgPath);

    ~MImage();

    sf::Texture* getSfTexture();
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
    MPoint             position = MPoint();
    sf::RenderTexture* texture  = nullptr;
    sf::Sprite*        sprite   = nullptr;
    sf::RenderWindow*  window   = nullptr;

public:
    explicit RenderTarget(MPoint _position, MPoint _size, sf::RenderWindow* _window);
    ~RenderTarget();

    MPoint getStart();
    MPoint getSize ();
    sf::RenderTexture* getRenderTexture();

    void drawLine  (MPoint start,  MPoint end,    MColor color);
    void drawRect  (MPoint start,  MPoint size,   MColor fillColor, MColor outColor);
    void drawCircle(MPoint centre, double radius, MColor color);
    void drawSprite(MPoint start,  MPoint size,   MImage* img); 
    void drawText  (MPoint start,  const char* text, MColor color, MFont* font, unsigned pt);
    void setPixel  (MPoint pos, MColor color);
};

#endif