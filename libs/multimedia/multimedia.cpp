#include "multimedia.h"

double lerp(double t1, double t2, double t3) {
    return t1 + t3 * (t2 - t1);
}

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

bool MPoint::isNan() {
    return std::isnan(x) || std::isnan(y);
}

double MPoint::getLen() {
    return sqrt(x * x + y * y);
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
    return fabs(a.x - b.x) < EPSILON && fabs(a.y - b.y) < EPSILON;
}

void operator+=(MColor& a, const int b) {
    a.r = std::min(255, a.r + b);
    a.g = std::min(255, a.g + b);
    a.b = std::min(255, a.b + b);
}

double operator| (const MPoint& a, const MPoint& b) {
    return a.x * b.x + a.y * b.y;
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

sf::Color MColor::toSfColor() {
    return sf::Color(r, g, b, a);
}

bool operator==(const MColor& a, const MColor& b) {
    return a.a == b.a && a.r == b.r && a.g == b.g && a.b == b.b;
}

MPoint getSymbolSize(char symbol, MFont* font, int pt) {
    ON_ERROR(!font, "Font ptr was null!", MPoint());

    char str[2] = {symbol, '\0'};
    sf::Text text(str, *font->getSfFont(), pt);

    return MPoint(text.getLocalBounds().getSize());
}

MPoint getTextSize(char* text,  MFont* font, int pt) {
    ON_ERROR(!font, "Font ptr was null!", MPoint());

    sf::Text sfText(text, *font->getSfFont(), pt);
    return MPoint(sfText.getLocalBounds().getSize());
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

        bool hasLoaded = img->loadFromFile(_imgPath);
        ON_ERROR(!hasLoaded, "No such file!",);
    }

MImage::MImage(sf::Texture* _imgText) :
    img(_imgText)   {}

MImage::~MImage() {
    if (img) delete img;

    img     = nullptr;
    imgPath = nullptr;
}

void MImage::imgFromPixel(List<List<MColor>*>* pixels) {
    ON_ERROR(!pixels, "List was null!",);

    size_t xSize = pixels      ->getSize();
    size_t ySize = (*pixels)[0]->getSize();

    sf::Image sfImg;
    sfImg.create(xSize, ySize, sf::Color::Transparent);

    for (size_t i = 0; i < xSize; i++) {
        for (size_t j = 0; j < ySize; j++) {
            sfImg.setPixel(i, j, (*(*pixels)[i])[j].toSfColor());
        }
    }

    delete img;

    img = new sf::Texture();
    img->loadFromImage(sfImg);
}

MColor MImage::getPixel(MPoint pos) {
    sf::Image sfPixels = img->copyToImage();

    return MColor(sfPixels.getPixel(pos.x, pos.y));
}

List<List<MColor>*>* MImage::getPixels() {
    ON_ERROR(!img, "Texture pointer was null!", nullptr);

    sf::Image sfPixels = img->copyToImage();

    size_t xSize = sfPixels.getSize().x;
    size_t ySize = sfPixels.getSize().y;

    // init list
    List<List<MColor>*>* res = new List<List<MColor>*>(xSize);
    for (size_t i = 0; i < xSize; i++) {
        res->pushBack(new List<MColor>(ySize));
    }

    for (size_t i = 0; i < xSize; i++) {
        for (size_t j = 0; j < ySize; j++) {
            (*res)[i]->pushBack(MColor(sfPixels.getPixel(i, j)));
        }
    }

    return res;
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

sf::Sprite* RenderTarget::getSprite() {
    return sprite;
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

MPoint RenderTarget::getPosition() {
    return position;
}

MPoint RenderTarget::getStart() {
    sf::Vector2f pos = sprite->getPosition();
    return MPoint(pos);
}

MPoint RenderTarget::getSize () {
    sf::Vector2u size = texture->getSize();
    return MPoint(size);
}

void RenderTarget::setTexture(MImage* const mImage) {
    ON_ERROR(!mImage, "Texture ptr was null!",);

}

void RenderTarget::clear(MColor col) {
    if (texture) texture->clear(col.toSfColor());
}

MImage* RenderTarget::getImage() {
    sf::Texture *imgTexture = new sf::Texture(texture->getTexture());
    return new MImage(imgTexture);
}

void RenderTarget::_drawLine(MPoint start, MPoint end, MColor color) {
    ON_ERROR(!texture, "Drawable area was null!",);

    sf::VertexArray line(sf::LinesStrip, 2);
    line[0].position = start.toSfVector();
    line[0].color    = color.toSfColor ();

    line[1].position = end  .toSfVector();
    line[1].color    = color.toSfColor ();

    texture->draw(line);
    texture->display();
    // if (sprite) window ->draw(*sprite);
}

void RenderTarget::drawLine(MPoint start, MPoint end, MColor color, RegionSet* regions) {
    ON_ERROR(!texture, "Drawable area was null!",);

    if (regions) {
        size_t regCnt = regions->getSize();

        for (size_t i = 0; i < regCnt; i++) {
            MathRectangle drawZone = (*regions)[i];

            MPoint resStart, resEnd;
            if (intersectLineRectangle(start, end, drawZone, resStart, resEnd)) {
                _drawLine(resStart, resEnd, color);
            }
        }
    } else {
        _drawLine(start, end, color);
    }
}

void RenderTarget::_drawRect(MPoint start, MPoint size, MColor fillColor, MColor outColor) {
    ON_ERROR(!texture, "Drawable area was null!",);

    sf::RectangleShape rect(size.toSfVector());
    rect.setPosition(start.toSfVector());
    rect.setFillColor(fillColor.toSfColor());
    rect.setOutlineColor(outColor.toSfColor());
    rect.setOutlineThickness(OUTLINE_THICKNESS);

    texture->draw(rect);
    texture->display();
    // if (sprite) window ->draw(*sprite);
}

void RenderTarget::drawRect(MPoint start, MPoint size, MColor fillColor, MColor outColor, RegionSet* regions) {
    ON_ERROR(!texture, "Drawable area was null!",);

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

void RenderTarget::_drawCircle(MPoint lu, double radius, MColor color, MColor fillColor) {
    ON_ERROR(!texture, "Drawable area was null!",);

    sf::CircleShape circle    (radius);
    circle.setPosition        (lu.toSfVector());
    circle.setFillColor       (fillColor.toSfColor());
    circle.setOutlineThickness(LINE_DIAM);
    circle.setOutlineColor    (color.toSfColor());

    texture->draw(circle);
    texture->display();
    // if (sprite) window ->draw(*sprite);
}

void RenderTarget::drawCircle(MPoint lu, double radius, MColor color, RegionSet* regions, MColor fillColor) {
    ON_ERROR(!texture, "Drawable area was null!",);

    if (regions) {
        
    } else {
        _drawCircle(lu, radius, color, fillColor);
    }
}

void RenderTarget::_drawSprite(MPoint start,  MPoint size,  MImage* img) {
    ON_ERROR(!texture, "Drawable area was null!",);

    sf::RectangleShape rect(size.toSfVector());
    rect.setPosition(start.toSfVector());
    rect.setTexture((*img).getSfTexture());

    texture->draw(rect);
    texture->display();
    // if (sprite) window ->draw(*sprite);
}

void RenderTarget::drawSprite(MPoint start, MPoint size, MImage* img, RegionSet* regions) {
    ON_ERROR(!texture, "Drawable area was null!",);

    if (!regions) {
        _drawSprite(start, size, img);
        return;
    }

    if (!img->getSfTexture()) return;

    List<MathRectangle>* rects = regions->getRectangles();
    ON_ERROR(!rects, "List was nullptr!",);

    sf::Sprite toCrop(*img->getSfTexture());
    toCrop.setPosition(start.x, start.y);

    sf::RenderTexture tmp;
    tmp.create(texture->getSize().x, texture->getSize().y);
    tmp.draw(toCrop);
    tmp.display();

    size_t rectSize = rects->getSize();
    for (size_t i = 0; i < rectSize; i++) {
        MPoint rectPos  = (*rects)[i].getPosition();
        MPoint rectSize = (*rects)[i].getSize();

        if (!rectPos.isNan() && !rectSize.isNan()) {
            sf::Sprite drawSprite(tmp.getTexture(), sf::IntRect(rectPos.x, rectPos.y, rectSize.x, rectSize.y));
            drawSprite.setPosition(rectPos.toSfVector());
    
            texture->draw(drawSprite);
        }
    }

    texture->display();
    // window ->draw(*sprite);
}

void RenderTarget::drawText(MPoint start,  const char* text, MColor color, MFont* font, unsigned pt, RegionSet* regions) {
    ON_ERROR(!texture || !sprite, "Drawable area was null!",);
    ON_ERROR(!text,    "String was null!",);

    sf::Font* drawFont = (*font).getSfFont();
    ON_ERROR(!drawFont, "Font ptr was null",)
    
    sf::Text drawText = sf::Text(text, *drawFont, pt);
    drawText.setFillColor(color.toSfColor());
    drawText.setPosition(start.toSfVector());

    texture->draw(drawText);
    texture->display();
    // window ->draw(*sprite);
}

void RenderTarget::drawFrame(MPoint start,  MPoint size,   MColor outColor, RegionSet* regions) {
    drawLine(start,        MPoint(start.x, start.y + size.y), outColor, regions);
    drawLine(start,        MPoint(start.x + size.x, start.y), outColor, regions);
    drawLine(MPoint(start.x + size.x, start.y), MPoint(start.x + size.x, start.y + size.y), outColor, regions);
    drawLine(MPoint(start.x, start.y + size.y), MPoint(start.x + size.x, start.y + size.y), outColor, regions);
}

void RenderTarget::setPixel(MPoint pos, MColor color, RegionSet* regions) {
    ON_ERROR(!texture, "Drawable area was null!",);

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
    // if (sprite) window ->draw(*sprite);
}

void RenderTarget::drawEllipse(MPoint pos, double scaleX, double scaleY, double maxRad, MColor color) {
    ON_ERROR(!texture, "Drawable area was null!",);

    sf::CircleShape ellipse    (maxRad);
    ellipse.setPosition        (pos.toSfVector());
    ellipse.setFillColor       (sf::Color::Transparent);
    ellipse.setOutlineThickness(LINE_DIAM);
    ellipse.setOutlineColor    (color.toSfColor());

    ellipse.setScale(scaleX, scaleY);

    texture->draw(ellipse);
    texture->display();
    // if (sprite) window ->draw(*sprite);
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

void MathRectangle::move(MPoint delta) {
    position += delta;
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

INTERSECTION_TYPE getIntersLineLine(MPoint lineStart1, MPoint lineEnd1, MPoint lineStart2, MPoint lineEnd2, MPoint &intersPoint) {
    // build 1 segment a1x + b1y = c1
    double a1 = lineEnd1.y        - lineStart1.y;
    double b1 = lineStart1.x      - lineEnd1.x;
    double c1 = lineStart1.x * a1 + lineStart1.y * b1;

    // build 2 segment a2x + b2y = c2
    double a2 = lineEnd2.y        - lineStart2.y;
    double b2 = lineStart2.x      - lineEnd2.x;
    double c2 = lineStart2.x * a2 + lineStart2.y * b2;

    // x = (c1*b2 - c2*b1) / (a1*b2 - a2*b1)
    // y = (a1*c2 - a2*c1) / (a1*b2 - a2*b1)
    double delta = a1 * b2 - a2 * b1;

    // parallel
    if (fabs(delta) < EPSILON) {
        if (lineStart1.x == lineStart2.x && lineStart1.y == lineStart2.y &&
            lineEnd1  .x == lineEnd2  .x && lineEnd1  .y == lineEnd2  .y)
            return SAME;

        return NONE;
    }

    double x = (c1 * b2 - c2 * b1) / delta;
    double y = (c2 * a1 - c1 * a2) / delta;

    intersPoint = MPoint(x, y);

    // figure out that point is inside segment
    if ((std::min(lineStart1.x, lineEnd1.x) <= x && x <= std::max(lineStart1.x, lineEnd1.x) &&
         std::min(lineStart1.y, lineEnd1.y) <= y && y <= std::max(lineStart1.y, lineEnd1.y)) &&
       ( std::min(lineStart2.x, lineEnd2.x) <= x && x <= std::max(lineStart2.x, lineEnd2.x)  &&
         std::min(lineStart2.y, lineEnd2.y) <= y && y <= std::max(lineStart2.y, lineEnd2.y)))
        return POINT;

    return NONE;
}

bool intersectLineRectangle(MPoint &lineStart, MPoint &lineEnd, MathRectangle rect, MPoint &resStart, MPoint &resEnd) {
    MPoint intersPoints[2];
    int pointCnt = 0;

    MPoint testStart = MPoint(rect.l(), rect.top()), testEnd = MPoint(rect.r(), rect.top());
    INTERSECTION_TYPE intersRes = getIntersLineLine(lineStart, lineEnd, testStart, testEnd, intersPoints[pointCnt]);
    if (intersRes == SAME) {
        resStart = testStart; resEnd = testEnd;
        return true;
    }
    else if (intersRes == POINT) {
        pointCnt++;
    }

    testStart = MPoint(rect.l(), rect.down()), testEnd = MPoint(rect.r(), rect.down());
    intersRes = getIntersLineLine(lineStart, lineEnd, testStart, testEnd, intersPoints[pointCnt]);
    if (intersRes == SAME) {
        resStart = testStart; resEnd = testEnd;
        return true;
    }
    else if (intersRes == POINT) {
        pointCnt++;
        if (pointCnt == 2) {
            resStart = intersPoints[0], resEnd = intersPoints[1];
            return true;
        }
    }

    testStart = MPoint(rect.l(), rect.top()), testEnd = MPoint(rect.l(), rect.down());
    intersRes = getIntersLineLine(lineStart, lineEnd, testStart, testEnd, intersPoints[pointCnt]);
    if (intersRes == SAME) {
        resStart = testStart; resEnd = testEnd;
        return true;
    }
    else if (intersRes == POINT) {
        pointCnt++;
        if (pointCnt == 2) {
            resStart = intersPoints[0], resEnd = intersPoints[1];
            return true;
        }
    }

    testStart = MPoint(rect.r(), rect.top()), testEnd = MPoint(rect.r(), rect.down());
    intersRes = getIntersLineLine(lineStart, lineEnd, testStart, testEnd, intersPoints[pointCnt]);
    if (intersRes == SAME) {
        resStart = testStart; resEnd = testEnd;
        return true;
    }
    else if (intersRes == POINT) {
        pointCnt++;
        if (pointCnt == 2) {
            resStart = intersPoints[0], resEnd = intersPoints[1];
            return true;
        }
    }

    return false;
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
        renderTarget->drawRect(rect.getPosition(), rect.getSize(), debCol, MColor(BLACK));
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
        if (!(*b)[i].getSize().isNan()) addRegion((*b)[i]);
    }
}

RegionSet* RegionSet::cross(const RegionSet* b) {
    RegionSet* newSet = new RegionSet();

    size_t listSize = getSize();
    size_t bListSize = b->rectangles->getSize();
    for (size_t i = 0; i < listSize; i++) {
        for (size_t j = 0; j < bListSize; j++) {
            // std::cout << (*rectangles)[i].getSize().x << ' ' << (*rectangles)[i].getSize().y << '\n';
            // std::cout << (*b->rectangles)[j].getSize().x << ' ' << (*b->rectangles)[j].getSize().y << '\n';

            MathRectangle inters = getIntersection((*rectangles)[i], (*b->rectangles)[j]);

            // std::cout << inters.getSize().x << ' ' << inters.getSize().y << "\n\n";

            RegionSet tmp = RegionSet();
            tmp.addRegion(inters);

            newSet->merge(&tmp);
        }
    }
    
    return newSet;
}