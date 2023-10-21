#ifndef _SUPPORT_h_
#define _SUPPORT_h_

#include "../../src/includes.h"

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

    ~MPoint();

    sf::Vector2f toSfVector();

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

    ~MColor();

    sf::Color toSfColor();
    friend bool operator==(const MColor& a, const MColor& b);
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
    explicit MImage(sf::Texture* _imgText);

    ~MImage();

    void imgFromPixel(List<List<MColor>*>* pixels);

    List<List<MColor>*>*  getPixels   ();
    sf::Texture*          getSfTexture();
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
    explicit RenderTarget(MPoint _position, MPoint _size);
    explicit RenderTarget(MPoint _position, MPoint _size, sf::RenderWindow* _window);
    ~RenderTarget();

    MPoint getPosition();
    MPoint getStart   ();
    MPoint getSize    ();
    sf::RenderTexture* getRenderTexture();
    sf::RenderWindow * getRenderWindow ();

    void    setTexture(MImage* mImage);
    void    clear(MColor col = MColor(TRANSPARENT));
    MImage* getImage();

    void _drawLine  (MPoint start,  MPoint end,    MColor color);
    void drawLine   (MPoint start,  MPoint end,    MColor color, RegionSet* regions = nullptr);
    void _drawRect  (MPoint start,  MPoint size,   MColor fillColor, MColor outColor);
    void drawRect   (MPoint start,  MPoint size,   MColor fillColor, MColor outColor, RegionSet* regions = nullptr);
    void _drawCircle(MPoint lu,     double radius, MColor color, MColor fillColor = MColor(TRANSPARENT));
    void drawCircle (MPoint lu,     double radius, MColor color, RegionSet* regions = nullptr, MColor fillColor = MColor(TRANSPARENT));
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