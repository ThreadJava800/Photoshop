#include "multimedia.h"

MPoint::MPoint() :
    x(NAN),
    y(NAN)        {}

MPoint::MPoint(double _x, double _y) :
    x(_x),
    y(_y)         {}

MPoint::MPoint(sf::Vector2f _point)  :
    x(_point.x),
    y(_point.y)   {}

MPoint::MPoint(sf::Vector2i _point) :
    x(_point.x),
    y(_point.y)   {}

MPoint::MPoint(sf::Vector2u _point) :
    x(_point.x),
    y(_point.y)   {}

MPoint::~MPoint() {
    x = NAN;
    y = NAN;
}

sf::Vector2f MPoint::toSfVector() {
    return sf::Vector2f(x, y);
}

void operator+=(MPoint& a, const MPoint& b) {
    a.x += b.x;
    a.y += b.y;
}

MPoint operator+ (const MPoint& a, const MPoint& b) {
    MPoint res = a;
    res += b;

    return res;
}

void operator-=(MPoint& a, const MPoint& b) {
    a.x -= b.x;
    a.y -= b.y;
}

MPoint operator-(const MPoint& a, const MPoint& b) {
    MPoint res = a;
    res -= b;

    return res;
}

void operator*=(MPoint& a, const double b) {
    a.x *= b;
    a.y *= b;
}

MPoint operator*(const MPoint& a, const double b) {
    MPoint res = a;
    res *= b;

    return res;
}

bool operator==(const MPoint& a, const MPoint& b) {
    return (a.x - b.x) < EPSILON && (a.y - b.y) < EPSILON;
}

MColor::MColor() :
    r(0),
    g(0),
    b(0),
    a(0)          {}

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

MFont::MFont() :
    font    (nullptr),
    fontFile(nullptr)   {}

MFont::MFont(const char* _fontFile) :
    fontFile(_fontFile) {
        font = new sf::Font();
        font->loadFromFile(_fontFile);
    }

MFont::~MFont() {
    if (font) delete font;

    font     = nullptr;
    fontFile = nullptr;
}

sf::Font* MFont::getSfFont() {
    return font;
}

MImage::MImage() :
    img    (nullptr),
    imgPath(nullptr)    {}

MImage::MImage(const char* _imgPath) :
    imgPath(_imgPath) {
        img = new sf::Texture();
        ON_ERROR(!img, "Unable to alloc memory",);

        ON_ERROR(!img->loadFromFile(_imgPath), "No such file!",);
    }

MImage::MImage(sf::Texture* _imgText) :
    img(_imgText)   {}

MImage::~MImage() {
    if (img) delete img;

    img     = nullptr;
    imgPath = nullptr;
}

sf::Texture* MImage::getSfTexture() {
    return img;
}

RenderTarget::RenderTarget(MPoint _position, MPoint _size) :
    position(_position),
    window  (nullptr),
    sprite  (nullptr)    {
        texture = new sf::RenderTexture();
        ON_ERROR(!texture, "Cannot allocate memory!",);
        texture->create(_size.x, _size.y);
    }

RenderTarget::RenderTarget(MPoint _position, MPoint _size, sf::RenderWindow* _window) :
    position(_position),
    window  (_window)    {
        texture = new sf::RenderTexture();
        ON_ERROR(!texture, "Cannot allocate memory!",);
        texture->create(_size.x, _size.y);

        sprite = new sf::Sprite();
        ON_ERROR(!sprite, "Cannot allocate memory!",);
        sprite->setPosition(_position.x, _position.y);
        sprite->setTexture(texture->getTexture());
    }

sf::RenderTexture* RenderTarget::getRenderTexture() {
    return texture;
}

sf::RenderWindow* RenderTarget::getRenderWindow() {
    return window;
}

RenderTarget::~RenderTarget() {
    window = nullptr;

    if (texture) delete texture;
    texture = nullptr;

    if (sprite) delete sprite;
    sprite = nullptr;

    if (window) delete window;
    window = nullptr;
}

MPoint RenderTarget::getStart() {
    sf::Vector2f pos = sprite->getPosition();
    return MPoint(pos);
}

MPoint RenderTarget::getSize () {
    sf::Vector2u size = texture->getSize();
    return MPoint(size);
}

void RenderTarget::_drawLine(MPoint start, MPoint end, MColor color) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);

    sf::VertexArray line(sf::LinesStrip, 2);
    line[0].position = start.toSfVector();
    line[0].color    = color.toSfColor ();

    line[1].position = end  .toSfVector();
    line[1].color    = color.toSfColor ();

    texture->draw(line);
    texture->display();
    window ->draw(*sprite);
}

void RenderTarget::drawLine(MPoint start, MPoint end, MColor color, RegionSet* regions) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);

    // TODO
    // if (regions) {
    //     size_t regCnt = regions->getSize();

    //     for (size_t i = 0; i < regCnt; i++) {

    //     }
    // } else {
    //     _drawLine(start, end, color);
    // }
}

void RenderTarget::_drawRect(MPoint start, MPoint size, MColor fillColor, MColor outColor) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);

    sf::RectangleShape rect(size.toSfVector());
    rect.setPosition(start.toSfVector());
    rect.setFillColor(fillColor.toSfColor());
    rect.setOutlineColor(outColor.toSfColor());
    rect.setOutlineThickness(OUTLINE_THICKNESS);

    texture->draw(rect);
    texture->display();
    window ->draw(*sprite);
}

void RenderTarget::drawRect(MPoint start, MPoint size, MColor fillColor, MColor outColor, RegionSet* regions) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);

    if (regions) {
        MathRectangle thisRect = MathRectangle(start, size);
        size_t regCnt = regions->getSize();

        for (size_t i = 0; i < regCnt; i++) {
            MathRectangle drawZone = (*regions)[i];

            if (isIntersected(drawZone, thisRect)) {
                MathRectangle toDraw = getIntersection(drawZone, thisRect);
                _drawRect(toDraw.getPosition(), toDraw.getSize(), fillColor, outColor);
            }
        }

    } else {
        _drawRect(start, size, fillColor, outColor);
    }
}

void RenderTarget::_drawCircle(MPoint centre, double radius, MColor color) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);

    sf::CircleShape circle(radius);
    circle.setPosition    (centre.toSfVector() - sf::Vector2f(radius, radius));
    circle.setFillColor   (color.toSfColor());

    texture->draw(circle);
    texture->display();
    window ->draw(*sprite);
}

void RenderTarget::drawCircle(MPoint centre, double radius, MColor color, RegionSet* regions) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);

    if (regions) {
        
    } else {
        _drawCircle(centre, radius, color);
    }
}

void RenderTarget::_drawSprite(MPoint start,  MPoint size,   MImage* img) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);

    sf::RectangleShape rect(size.toSfVector());
    rect.setPosition(start.toSfVector());
    rect.setTexture((*img).getSfTexture());

    texture->draw(rect);
    texture->display();
    window ->draw(*sprite);
}

void RenderTarget::drawSprite(MPoint start, MPoint size, MImage* img, RegionSet* regions) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);

    // if (!regions) {
        _drawSprite(start, size, img);
        return;
    // }

    List<MathRectangle>* rects = regions->getRectangles();
    ON_ERROR(!rects, "List was nullptr!",);

    sf::RenderTexture tmp;
    tmp.create(texture->getSize().x, texture->getSize().y);
    tmp.clear(sf::Color::Transparent);

    sf::Sprite toCrop(*img->getSfTexture());
    toCrop.setPosition(position.x, position.y);

    tmp.draw(toCrop);
    tmp.display();

    size_t rectSize = rects->getSize();
    for (size_t i = 0; i < rectSize; i++) {
        if (img->getSfTexture()) {
            sf::Sprite drawSprite(tmp.getTexture(), sf::IntRect((*rects)[i].getPosition().x, (*rects)[i].getPosition().x, (*rects)[i].getSize().x, (*rects)[i].getSize().y));
            drawSprite.setPosition((*rects)[i].getPosition().toSfVector());

            texture->draw(drawSprite);
        }
    }

    texture->display();
    window ->draw(*sprite);
}

void RenderTarget::drawText(MPoint start,  const char* text, MColor color, MFont* font, unsigned pt, RegionSet* regions) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);
    ON_ERROR(!text,    "String was null!",);

    sf::Font* drawFont = (*font).getSfFont();
    ON_ERROR(!drawFont, "Font ptr was null",)
    
    sf::Text drawText = sf::Text(text, *drawFont, pt);
    drawText.setColor(color.toSfColor());
    drawText.setPosition(start.toSfVector());

    texture->draw(drawText);
    texture->display();
    window ->draw(*sprite);
}

void RenderTarget::setPixel(MPoint pos, MColor color, RegionSet* regions) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);

    if (regions) {
        size_t regCnt = regions->getSize();

        for (size_t i = 0; i < regCnt; i++) {
            if (!((*regions)[i].isPointInside(pos))) return;
        }
    }

    sf::VertexArray point(sf::Points, 1);
    point[0].position = pos.toSfVector();
    point[0].color    = color.toSfColor();

    texture->draw(point);
    texture->display();
    window ->draw(*sprite);
}

MathRectangle::MathRectangle(MPoint _pos, MPoint _size) :
    position(_pos),
    size    (_size)     {}

MathRectangle::~MathRectangle() {
    position = MPoint();
    size     = MPoint();
}

double MathRectangle::top() {
    return position.y;
}

double MathRectangle::l() {
    return position.x;
}

double MathRectangle::r() {
    return position.x + size.x;
}

double MathRectangle::down() {
    return position.y + size.y;
}

MPoint MathRectangle::getPosition() {
    return position;
}

MPoint MathRectangle::getSize() {
    return size;
}

bool MathRectangle::isYInside(double yPoint) {
    return top() <= yPoint && yPoint <= down();
}

bool MathRectangle::isXInside(double xPoint) {
    return l() <= xPoint && xPoint <= r();
}

bool MathRectangle::isPointInside(MPoint point) {
    return isYInside(point.y) && isXInside(point.x);
}

bool isIntersected(MathRectangle posOld, MathRectangle posNew) {
    return posOld   .l() < posNew   .r() &&
           posOld   .r() > posNew   .l() &&
           posOld .top() < posNew.down() &&
           posOld.down() > posNew .top();
}

MathRectangle getIntersection(MathRectangle posOld, MathRectangle posNew) {
    if (!isIntersected(posOld, posNew)) {
        return MathRectangle(MPoint(), MPoint());
    }

    MPoint oldLD = MPoint(posOld.l(), posOld.down());
    MPoint oldRH = MPoint(posOld.r(), posOld. top());
    MPoint newLD = MPoint(posNew.l(), posNew.down());
    MPoint newRH = MPoint(posNew.r(), posNew. top());

    MPoint resLD = MPoint(std::max(oldLD.x, newLD.x), std::min(oldLD.y, newLD.y));
    MPoint resRH = MPoint(std::min(oldRH.x, newRH.x), std::max(oldRH.y, newRH.y));
    MPoint resLT = MPoint(resLD.x, resRH.y);
    MPoint resRD = MPoint(resRH.x, resLD.y);

    if (resLD.x > resRH.x || resRH.y > resLD.y) return MathRectangle(MPoint(), MPoint());

    return MathRectangle(resLT, resRD - resLT);
}

RegionSet* merge(MathRectangle posOld, MathRectangle posNew) {
    RegionSet* differenceSet = diff(posOld, posNew);
    differenceSet->addRegion(posNew);
    return differenceSet;
}

RegionSet* diff(MathRectangle posOld, MathRectangle posNew) {
    RegionSet* differenceSet = new RegionSet();
    
    if (isIntersected(posOld, posNew)) {

        // posNew is lefter than posOld
        if (posOld.isXInside(posNew.r())) {
            MPoint regPos  = MPoint(posNew.r(), std::max(posOld.top(), posNew.top()));
            MPoint regSize = MPoint(posOld.r() - posNew.r(), 
                                    std::min(posOld.down(), posNew.down()) - std::max(posOld.top(), posNew.top()));


            if (regSize.x != 0 && regSize.y != 0)
                differenceSet->addRegion(MathRectangle(regPos, regSize));
        }

        // posNew is righter than posOld
        if (posOld.isXInside(posNew.l())) {
            MPoint regPos  = MPoint(posOld.l(), std::max(posOld.top(), posNew.top()));
            MPoint regSize = MPoint(posNew.l() - posOld.l(), 
                                    std::min(posOld.down(), posNew.down()) - std::max(posOld.top(), posNew.top()));

            if (regSize.x != 0 && regSize.y != 0)
                differenceSet->addRegion(MathRectangle(regPos, regSize));
        }

        // posNew is upper than posOld
        if (posOld.isYInside(posNew.down())) {
            MPoint regPos  = MPoint(posOld.l(), posNew.down());
            MPoint regSize = MPoint(posOld.size.x, posOld.down() - posNew.down());

            if (regSize.x != 0 && regSize.y != 0)
                differenceSet->addRegion(MathRectangle(regPos, regSize));
        }

        // posNew is lower than posOld
        if (posOld.isYInside(posNew.top())) {
            MPoint regPos  = MPoint(posOld.l(), posOld.top());
            MPoint regSize = MPoint(posOld.size.x, posNew.top() - posOld.top());

            if (regSize.x != 0 && regSize.y != 0)
                differenceSet->addRegion(MathRectangle(regPos, regSize));
        }
    } else {
        differenceSet->addRegion(posOld);
    }

    return differenceSet;
}

bool operator==(const MathRectangle a, const MathRectangle b) {
    return a.position == b.position && a.size == b.size;
}

bool operator!=(const MathRectangle a, const MathRectangle b) {
    return !(a == b);
}

RegionSet::RegionSet() {
    rectangles = new List<MathRectangle>();
}

RegionSet::~RegionSet() {
    delete rectangles;

    rectangles = nullptr;
}

size_t RegionSet::getSize() {
    return rectangles->getSize();
}

void RegionSet::addRegion(MathRectangle region) {
    rectangles->pushBack(region);
}

List<MathRectangle>* RegionSet::getRectangles() {
    return rectangles;
}

MathRectangle& RegionSet::operator[](const size_t index) const {
    return (*rectangles)[index];
}

void RegionSet::visualize(RenderTarget* renderTarget, MColor debCol) {
    ON_ERROR(!renderTarget, "Drawable area was null",);
    ON_ERROR(!rectangles, "List was null!",);

    size_t listSize = rectangles->getSize();

    for (size_t i = 0; i < listSize; i++) {
        MathRectangle rect = (*rectangles)[i];

        debCol.a = 255 / 2;
        renderTarget->drawRect(rect.getPosition(), rect.getSize(), debCol, debCol);
    }
}

void RegionSet::subtract(const RegionSet* b) {
    if(!rectangles || !b || !b->rectangles) return;

    List<MathRectangle>* newSet = new List<MathRectangle>();

    size_t aListSize = rectangles->getSize();
    size_t bListSize = b->rectangles->getSize();

    for (size_t i = 0; i < aListSize; i++) {
        for (size_t j = 0; j < bListSize; j++) {
            RegionSet* diffSet = diff((*rectangles)[i], (*b)[j]);

            for (size_t k = 0; k < diffSet->getSize(); k++) {
                newSet->pushBack((*diffSet)[k]);
            }

            delete diffSet;
        }
    }

    delete rectangles;
    rectangles = newSet;
}

void RegionSet::merge(const RegionSet* b) {
    if(!rectangles || !b || !b->rectangles) return;

    size_t listSize = b->rectangles->getSize();
    for (size_t i = 0; i < listSize; i++) {
        addRegion((*b)[i]);
    }
}

RegionSet* RegionSet::cross(const RegionSet* b) {
    RegionSet* newSet = new RegionSet();

    size_t listSize = getSize();
    size_t bListSize = b->rectangles->getSize();
    for (size_t i = 0; i < listSize; i++) {
        for (size_t j = 0; j < bListSize; j++) {
            MathRectangle inters = getIntersection((*rectangles)[i], (*b->rectangles)[j]);

            RegionSet tmp = RegionSet();
            tmp.addRegion(inters);

            newSet->merge(&tmp);
        }
    }
    
    return newSet;
}