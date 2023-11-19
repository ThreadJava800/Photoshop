#ifndef _SUPPORT_h_
#define _SUPPORT_h_

#include <SFML/Graphics.hpp>

#include "../../src/includes.h"
#include "../../src/plugin.h"

static const double EPSILON = 1e-12;

class RegionSet;
class MathRectangle;

double lerp(double t1, double t2, double t3);

struct MPoint {
    double x = 0;
    double y = 0;

    explicit MPoint();
    explicit MPoint(double _x, double _y);
    explicit MPoint(sf::Vector2f _point);
    explicit MPoint(sf::Vector2i _point);
    explicit MPoint(sf::Vector2u _point);
    explicit MPoint(plugin::Vec2 _point);

    ~MPoint();

    sf::Vector2f toSfVector();
    plugin::Vec2 toVec2    ();

    bool   isNan();
    double getLen();

    friend void   operator+=(      MPoint& a, const MPoint& b);
    friend MPoint operator+ (const MPoint& a, const MPoint& b);
    friend void   operator-=(      MPoint& a, const MPoint& b);
    friend MPoint operator- (const MPoint& a, const MPoint& b);
    friend void   operator*=(      MPoint& a, const double  b);
    friend MPoint operator* (const MPoint& a, const double  b);
    friend bool   operator==(const MPoint& a, const MPoint& b);
    friend double operator| (const MPoint& a, const MPoint& b); // dot product
};

struct MColor {
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

    explicit MColor();
    explicit MColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a);
    explicit MColor(sf::Color _color);
    explicit MColor(plugin::Color _color);

    sf::Color     toSfColor();
    plugin::Color toPlColor() const;
    friend bool operator==(const MColor& a, const MColor& b);
    friend void operator+=(      MColor& a, const int     b);

    static const MColor WHITE;
    static const MColor TRANSPARENT;
    static const MColor BLACK;
    static const MColor GRAY;
    static const MColor YELLOW;
    static const MColor RED;
    static const MColor MAGENTA;
    static const MColor GREEN;
    static const MColor BLUE;
    static const MColor CYAN;
    static const MColor LIGHT_BLUE;
};

static const MColor    DEFAULT_BACK_COL = MColor::WHITE;
static const MColor    DEFAULT_COLOR    = MColor::TRANSPARENT;

static const int DEB_COLS_CNT = 7;
static const MColor DEB_COLS[] = {
    MColor(sf::Color::Red),
    MColor(sf::Color::Yellow),
    MColor(sf::Color::Green),
    MColor(sf::Color::Blue),
    MColor(sf::Color::Cyan),
    MColor(sf::Color::Magenta),
    MColor(sf::Color::White)
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

MPoint getSymbolSize(char symbol, MFont* font, int pt);
MPoint getTextSize  (char* text,  MFont* font, int pt);

struct MImage {
private:
    sf::Texture* img = nullptr;

public:
    const char* imgPath = nullptr;

    explicit MImage();
    explicit MImage(const char* _imgPath);
    explicit MImage(sf::Texture* _imgText);
    explicit MImage(const plugin::Texture* _texture);

    ~MImage();

    void             buildFromPluginTexture(const plugin::Texture* _texture);
    plugin::Texture* toPluginTexture();

    void imgFromPixel(List<List<MColor>*>* pixels);
    bool saveToFile  (const char* fileName);

    MColor                getPixel    (MPoint pos);
    List<List<MColor>*>*  getPixels   ();
    sf::Texture*          getSfTexture();
};

enum MKeysIDS {
    BACKSPACE   = 8,
    RETURN      = 13,

    LEFT_KEY    = sf::Keyboard::Left,
    RIGHT_KEY   = sf::Keyboard::Right,
    DOWN_KEY    = sf::Keyboard::Down,
    UP_KEY      = sf::Keyboard::Up,

    DEFAULT_KEY = -1
};

struct MKeyboard {
    char     symbol;
    MKeysIDS keyId;

    explicit MKeyboard(char _symbol, MKeysIDS _keyId) : symbol(_symbol), keyId(_keyId) {}
};

enum MMouse {
    LEFT  = (int)(plugin::MouseButton::Left),
    RIGHT = (int)(plugin::MouseButton::Right)
};

enum WindowType {
    FULLSCREEN = sf::Style::Fullscreen,
    DEFAULT    = sf::Style::Default
};

class RenderTarget : public plugin::RenderTargetI {
private:
    MPoint             position = MPoint();
    sf::RenderTexture* texture  = nullptr;
    sf::Sprite*        sprite   = nullptr;
    sf::RenderWindow*  window   = nullptr;

public:
    explicit RenderTarget(MPoint _position, MPoint _size, bool needWindow = false, int winX = 1920, int winY = 1080, int posX = 0, int posY = 0, WindowType winType = FULLSCREEN);
    ~RenderTarget();

    MPoint getPosition();
    MPoint getStart   ();
    MPoint getSize    ();
    sf::RenderTexture* getRenderTexture();
    sf::RenderWindow * getRenderWindow ();
    sf::Sprite       * getSprite       ();

    void    setTexture  (MImage* mImage);
    void    clearTexture(MColor col = MColor::TRANSPARENT);
    MImage* getImage();

    void clearAll  ();
    void displayAll();

    // plugin support funcs

    void setPixel   (plugin::Vec2 pos, plugin::Color color)                                          override;
    void drawLine   (plugin::Vec2 pos, plugin::Vec2 point1, plugin::Color color)                     override;
    void drawRect   (plugin::Vec2 pos, plugin::Vec2 size,   plugin::Color color)                     override;
    void drawEllipse(plugin::Vec2 pos, plugin::Vec2 size,   plugin::Color color)                     override;
    void drawTexture(plugin::Vec2 pos, plugin::Vec2 size, const plugin::Texture *texture)            override;
    void drawText   (plugin::Vec2 pos, const char *content, uint16_t char_size, plugin::Color color) override;

    plugin::Texture *getTexture() override;
    void display()                override;
    void clear()                  override;

    //

    void _drawLine  (MPoint start,  MPoint end,    MColor color);
    void drawLine   (MPoint start,  MPoint end,    MColor color, RegionSet* regions = nullptr);
    void _drawRect  (MPoint start,  MPoint size,   MColor fillColor, MColor outColor);
    void drawRect   (MPoint start,  MPoint size,   MColor fillColor, MColor outColor, RegionSet* regions = nullptr);
    void _drawCircle(MPoint lu,     double radius, MColor color, MColor fillColor = MColor::TRANSPARENT);
    void drawCircle (MPoint lu,     double radius, MColor color, RegionSet* regions = nullptr, MColor fillColor = MColor::TRANSPARENT);
    void _drawSprite(MPoint start,  MPoint size,   MImage* img); 
    void drawSprite (MPoint start,  MPoint size,   MImage* img, RegionSet* regions = nullptr); 
    void _drawText  (MPoint start,  MPoint size,   MImage* img); 
    void drawText   (MPoint start,  const char* text, MColor color, MFont* font, unsigned pt, RegionSet* regions = nullptr);
    void drawFrame  (MPoint start,  MPoint size,   MColor outColor, RegionSet* regions = nullptr);
    void setPixel   (MPoint pos, MColor color, RegionSet* regions = nullptr);

    void drawEllipse(MPoint pos, double scaleX, double scaleY, double maxRad, MColor color);
};

enum INTERSECTION_TYPE {
    NONE,
    POINT,
    SAME
};

class MathRectangle {
private:
    MPoint position = MPoint();
    MPoint size     = MPoint();

public:
    explicit MathRectangle(MPoint _pos, MPoint _size);
    ~MathRectangle();

    double top();
    double l();
    double r();
    double down(); 

    MPoint getPosition();
    MPoint getSize    ();

    void move(MPoint delta);

    bool isYInside(double yPoint);
    bool isXInside(double xPoint);
    bool isPointInside(MPoint point);

    friend INTERSECTION_TYPE getIntersLineLine(MPoint lineStart1, MPoint lineEnd1, MPoint lineStart2, MPoint lineEnd2, MPoint &intersPoint);

    friend bool intersectLineRectangle(MPoint &lineStart, MPoint &lineEnd, MathRectangle rect, MPoint &resStart, MPoint &resEnd);

    friend bool          isIntersected  (MathRectangle posOld, MathRectangle posNew);
    friend MathRectangle getIntersection(MathRectangle posOld, MathRectangle posNew);
    friend RegionSet*    merge          (MathRectangle posOld, MathRectangle posNew);
    friend RegionSet*    diff           (MathRectangle posOld, MathRectangle posNew);

    friend bool operator==(const MathRectangle a, const MathRectangle b);
    friend bool operator!=(const MathRectangle a, const MathRectangle b);
};

class RegionSet {
private:
   List<MathRectangle>* rectangles = nullptr;

public:
    explicit RegionSet();
    ~RegionSet();

    void addRegion(MathRectangle region);

    size_t               getSize      ();
    List<MathRectangle>* getRectangles();

    MathRectangle& operator[](const size_t index) const;
    void visualize(RenderTarget* renderTarget, MColor debCol);

    void       subtract(const RegionSet* b);
    void       merge   (const RegionSet* b);
    RegionSet* cross   (const RegionSet* b);
};

#endif