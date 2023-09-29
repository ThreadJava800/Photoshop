#include "multimedia.h"

MPoint::MPoint(double _x, double _y) :
    x(_x),
    y(_y)         {}

MPoint::MPoint(sf::Vector2f _point)  :
    x(_point.x),
    y(_point.y)   {}

MPoint::~MPoint() {
    x = 0;
    y = 0;
}

sf::Vector2f MPoint::toSfVector() {
    return sf::Vector2f(x, y);
}

MColor::MColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) :
    r(_r),
    g(_g),
    b(_b),
    a(_a)         {}

MColor::MColor(sf::Color _color) :
    r(_color.r),
    g(_color.g),
    b(_color.b),
    a(_color.a)   {}

MColor::~MColor() {
    r = 0;
    g = 0;
    b = 0;
    a = 0;
}

sf::Color MColor::toSfColor() {
    return sf::Color(r, g, b, a);
}

MFont::MFont(const char* _fontFile) :
    fontFile(_fontFile) {
        font = new sf::Font();
        font->loadFromFile(_fontFile);
    }

MFont::~MFont() {
    delete font;

    font     = nullptr;
    fontFile = nullptr;
}

sf::Font* MFont::getSfFont() {
    return font;
}

RenderTarget::RenderTarget(sf::RenderTexture* _sprite, sf::RenderTexture* _texture) :
    sprite (_sprite),
    texture(_texture)   {}

RenderTarget::~RenderTarget() {
    sprite  = nullptr;
    texture = nullptr;
}

void RenderTarget::drawLine(MPoint start, MPoint end, MColor color) {
    ON_ERROR(!texture, "Texture ptr was null!",);

    sf::VertexArray line(sf::LinesStrip, 2);
    line[0].position = start.toSfVector();
    line[0].color    = color.toSfColor ();

    line[1].position = end  .toSfVector();
    line[1].color    = color.toSfColor ();

    texture->draw(line);
}

void RenderTarget::drawRect(MPoint start, MPoint size, MColor color) {
    ON_ERROR(!texture, "Texture ptr was null!",);

    sf::RectangleShape rect(start.toSfVector());
    rect.setSize(size.toSfVector());
    rect.setFillColor(color.toSfColor());

    texture->draw(rect);
}

void RenderTarget::drawCircle(MPoint centre, double radius, MColor color) {
    ON_ERROR(!texture, "Texture ptr was null!",);

    sf::CircleShape circle(radius);
    circle.setPosition    (centre.toSfVector() - sf::Vector2f(radius, radius));
    circle.setFillColor   (color.toSfColor());

    texture->draw(circle);
}

void RenderTarget::drawText(MPoint start,  const char* text, MColor color, MFont font, unsigned pt) {
    ON_ERROR(!texture, "Texture ptr was null!",);
    ON_ERROR(!text,    "String was null!",);

    sf::Font* drawFont = font.getSfFont();
    ON_ERROR(!drawFont, "Font ptr was null",)
    
    sf::Text drawText = sf::Text(text, *drawFont, pt);
    texture->draw(drawText);
}

void RenderTarget::setPixel(MPoint pos, MColor color) {
    ON_ERROR(!texture, "Texture ptr was null!",);

    sf::VertexArray point(sf::Points, 1);
    point[0].position = pos.toSfVector();
    point[0].color    = color.toSfColor();

    texture->draw(point);
}