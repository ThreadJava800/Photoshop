#include "canvas.h"

Tool::Tool() :
    start(MPoint()),
    last (MPoint())     {}

Tool::Tool(MPoint _start, MPoint _end) :
    start(_start),
    last (_end)         {}

Brush::Brush() :
    Tool()  {
        points = new List<MPoint>();
    }

Brush::Brush(MPoint _start, MPoint _end) :
    Tool(_start, _end)  {
        points = new List<MPoint>();
    }

Brush::~Brush() {
    delete points;
}

double Brush::getCatmullCoeff(double prevCoeff, MPoint p1, MPoint p2) {
    MPoint diffPoint = p2 - p1;

    return std::pow((diffPoint | diffPoint), CATMULL_ALPHA * 0.5) + prevCoeff;
}

List<MPoint>* Brush::getCatmullCoeffs(MPoint p0, MPoint p1, MPoint p2, MPoint p3, bool setOf3) {
    ON_ERROR(!points, "Point list ptr area was nullptr!", nullptr);

    double t0 = 0;
    double t1 = getCatmullCoeff(t0, p0, p1);
    double t2 = getCatmullCoeff(t1, p1, p2);
    double t3 = getCatmullCoeff(t2, p2, p3);

    List<MPoint>* coeffs = new List<MPoint>();

    for (double i = 0; i < 1; i += 0.01) {
        double t  = lerp(t1, t2, i);

        if (t1 == t0 || t1 == t3 || t1 == t2 || t2 == t3 || t2 == t0) continue;

        MPoint a1 = p0 * ((t1 - t) / (t1 - t0)) + p1 * ((t - t0) / (t1 - t0));
        MPoint a2 = p1 * ((t2 - t) / (t2 - t1)) + p2 * ((t - t1) / (t2 - t1));
        MPoint a3 = p2 * ((t3 - t) / (t3 - t2)) + p3 * ((t - t2) / (t3 - t2));
        MPoint b1 = a1 * ((t2 - t) / (t2 - t0)) + a2 * ((t - t0) / (t2 - t0));
        if (setOf3) {
            coeffs->pushBack(b1);
            continue;
        }

        MPoint b2 = a2 * ((t3 - t) / (t3 - t1)) + a3 * ((t - t1) / (t3 - t1));
        MPoint c  = b1 * ((t2 - t) / (t2 - t1)) + b2 * ((t - t1) / (t2 - t1));

        coeffs->pushBack(c);
    }

    return coeffs;
}

void Brush::drawCatmullOf3(RenderTarget* perm, MColor color, MPoint p1, MPoint p2, MPoint p3) {
    ON_ERROR(!perm, "RenderTarget was null!",);

    List<MPoint>* drawPoints = getCatmullCoeffs(p1, p2, p3, MPoint(), true);
    size_t drawCnt = drawPoints->getSize();
        
    for (long i = 0; i < long(drawCnt); i++) {
        perm->drawCircle((*drawPoints)[i], LINE_DIAM, color, nullptr, color);
    }

    delete drawPoints;
}

void Brush::drawCatmull(RenderTarget* perm, MColor color) {
    ON_ERROR(!perm, "RenderTarget was null!",);

    size_t pointCnt = points->getSize();

    if (pointCnt == 1) {
        perm->drawCircle((*points)[0], LINE_DIAM, color, nullptr, color);
    }
    if (pointCnt == 2) {
        perm->drawLine((*points)[0], (*points)[1], color);
    }
    if (pointCnt == 3) {
        drawCatmullOf3(perm, color, (*points)[2], (*points)[1], (*points)[0]);
        drawCatmullOf3(perm, color, (*points)[0], (*points)[1], (*points)[2]);
    }

    if (pointCnt < 4) return;

    drawCatmullOf3(perm, color, (*points)[2], (*points)[1], (*points)[0]);
    for (long i = 0; i < long(pointCnt) - 3; i++) {
        List<MPoint>* drawPoints = getCatmullCoeffs((*points)[i], (*points)[i + 1], (*points)[i + 2], (*points)[i + 3]);

        size_t drawCnt = drawPoints->getSize();
        
        for (long i = 0; i < long(drawCnt) - 1; i++) {
            perm->drawCircle((*drawPoints)[i], LINE_DIAM, color, nullptr, color);
        }

        delete drawPoints;
    }

    drawCatmullOf3(perm, color, (*points)[pointCnt - 3], (*points)[pointCnt - 2], (*points)[pointCnt - 1]);
}

bool Brush::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    if (points->getSize() > 4) points->popFront();

    points->pushBack(cur);
    drawCatmull(perm, color);

    return true;
}

bool Brush::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    if (points->getSize() > 4) points->popFront();

    points->pushBack(cur);
    drawCatmull(perm, color);

    return true;
}

bool Brush::paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color) { 
    points->clear();

    return false; 
}

bool Brush::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) { 
    points->clear();

    return false; 
}

Spline::Spline() :
    Brush()             {}

Spline::Spline(MPoint _start, MPoint _end) :
    Brush(_start, _end) {}

bool Spline::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    if (btn == LEFT) {
        points->pushBack(cur);

        temp->clear();
        drawCatmull(temp, color);

        return true;
    }

    return paintOnDeactivate(perm, temp, color);
}


bool Spline::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    return true;
}

bool Spline::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    return true;
}

bool Spline::paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    temp->clear();
    drawCatmull(perm, color);

    points->clear();

    return false;
}

FillTool::FillTool() :
    Tool()              {}

FillTool::FillTool(MPoint _start, MPoint _end) :
    Tool(_start, _end)  {}

bool FillTool::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    MImage* textureImage              = perm->getImage();
    List<List<MColor>*>* pixelListPtr = textureImage->getPixels();
    if (!pixelListPtr) return false;

    size_t xSize =   pixelListPtr    ->getSize();
    size_t ySize = (*pixelListPtr)[0]->getSize();

    List<MPoint>  bfsList = List<MPoint>();
    List<bool>    was     = List<bool>(xSize * ySize);
    bfsList.pushBack(cur);

    List<List<MColor>*> resPixels = List<List<MColor>*>(xSize);
    for (size_t i = 0; i < xSize; i++) {
        resPixels.pushBack(new List<MColor>(ySize));
        for (size_t j = 0; j < ySize; j++) {
            resPixels[i]->pushBack(MColor());
        }
    }

    while (bfsList.getSize()) {
        MPoint bfs = bfsList.pop();

        if (was[bfs.y * xSize + bfs.x]) continue;
        was[bfs.y * xSize + bfs.x] = true;

        if (bfs.y != 0) {
            MPoint up = MPoint(bfs.x, bfs.y - 1);
            if ((*((*pixelListPtr)[up.x]))[up.y] == (*((*pixelListPtr)[bfs.x]))[bfs.y]) {
                bfsList.pushBack(up);

                (*(resPixels)[up.x])[up.y] = color;
            }
        }

        if (bfs.x != 0) {
            MPoint lp = MPoint(bfs.x - 1, bfs.y);
            if ((*((*pixelListPtr)[lp.x]))[lp.y] == (*((*pixelListPtr)[bfs.x]))[bfs.y]) {
                bfsList.pushBack(lp);

                (*(resPixels)[lp.x])[lp.y] = color;
            }
        }

        if (bfs.x != xSize - 1) {
            MPoint rp = MPoint(bfs.x + 1, bfs.y);
            if ((*((*pixelListPtr)[rp.x]))[rp.y] == (*((*pixelListPtr)[bfs.x]))[bfs.y]) {
                bfsList.pushBack(rp);

                (*(resPixels)[rp.x])[rp.y] = color;
            }
        }

        if (bfs.y != ySize - 1) {
            MPoint bp = MPoint(bfs.x, bfs.y + 1);
            if ((*((*pixelListPtr)[bp.x]))[bp.y] == (*((*pixelListPtr)[bfs.x]))[bfs.y]) {
                bfsList.pushBack(bp);

                (*(resPixels)[bp.x])[bp.y] = color;
            }
        }
    }

    textureImage->imgFromPixel(&resPixels);
    perm->drawSprite(MPoint(0, 0), MPoint(xSize, ySize), textureImage);

    delete textureImage;

    for (size_t i = 0; i < xSize; i++) delete (*pixelListPtr)[i];
    delete pixelListPtr;

    for (size_t i = 0; i < xSize; i++) delete resPixels[i];

    return true;
}

bool FillTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    return false;
}

bool FillTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    return false;
}

bool FillTool::paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color) {
    return false;
}

StraightTool::StraightTool() :
    Tool(),
    rectStart(MPoint()) {}

StraightTool::StraightTool(MPoint _start, MPoint _end) :
    Tool(_start, _end),
    rectStart(MPoint()) {}

bool StraightTool::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    if (btn == LEFT) {
        rectStart = cur;
        return true;
    }

    return false;
}

bool StraightTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    return true;
}

bool StraightTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    return false;
}

bool StraightTool::paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color) { return false; }

SquareTool::SquareTool() :
    StraightTool()             {}

SquareTool::SquareTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end) {}

void SquareTool::drawSquare(MPoint lu, MPoint cur, MColor color, RenderTarget *drawTarget) {
    ON_ERROR(!drawTarget, "Drawable area was null!",);

    MPoint size = MPoint(fabs(cur.x - lu.x), fabs(cur.y - lu.y));

    double xMin   = std::min(cur.x, lu.x);
    double yMin   = std::min(cur.y, lu.y);
    MPoint rectLU = MPoint(xMin, yMin);

    drawTarget->drawRect(rectLU, size, MColor(TRANSPARENT), color);
}

bool SquareTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "Drawable area was null!", false);

    temp->clear();
    drawSquare(rectStart, cur, color, temp);

    return true;
}

bool SquareTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "Drawable area was null!", false);

    temp->clear();
    drawSquare(rectStart, cur, color, perm);

    return false;
}

EllipseTool::EllipseTool() :
    StraightTool()              {}

EllipseTool::EllipseTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end)  {}

void EllipseTool::drawEllipse(MPoint lu, MPoint cur, MColor color, RenderTarget *drawTarget) {
    ON_ERROR(!drawTarget, "Drawable area was null!",);

    double xMin     = std::min(cur.x, lu.x);
    double yMin     = std::min(cur.y, lu.y);
    MPoint circleLU = MPoint(xMin, yMin);

    double height = fabs(cur.y - lu.y) / 2;
    double length = fabs(cur.x - lu.x) / 2;

    if (height < EPSILON || length < EPSILON) return;

    double scaleX = 1, scaleY = 1;
    if (length < height) scaleX = length / height;
    else                 scaleY = height / length;

    drawTarget->drawEllipse(circleLU, scaleX, scaleY, std::max(height, length), color);
}

bool EllipseTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "Drawable area was null!", false);

    temp->clear();
    drawEllipse(rectStart, cur, color, temp);

    return true;
}

bool EllipseTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "Drawable area was null!", false);

    temp->clear();
    drawEllipse(rectStart, cur, color, perm);

    return false;
}

LineTool::LineTool() :
    StraightTool()              {}

LineTool::LineTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end)  {}

bool LineTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "Drawable area was null!", false);

    temp->clear();
    temp->drawLine(rectStart, cur, color);

    return true;
}

bool LineTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "Drawable area was null!", false);

    temp->clear();
    perm->drawLine(rectStart, cur, color);

    return false;
}

CurveTool::CurveTool() :
    StraightTool() {
        points = new List<MPoint>();
    }

CurveTool::CurveTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end) {
        points = new List<MPoint>();
    }

CurveTool::~CurveTool() {
    delete points;
}

void CurveTool::drawCurve(MColor color, RenderTarget *drawTarget) {
    ON_ERROR(!drawTarget || !points, "Drawable area was null!",);

    size_t listSize = points->getSize();

    if (listSize == 1) {
        drawTarget->setPixel((*points)[0], color);
        return;
    }

    for (size_t i = 1; i < listSize; i++) {
        drawTarget->drawLine((*points)[i - 1], (*points)[i], color);
    }
}

bool CurveTool::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp || !points, "Drawable area was null!", false);

    if (btn == LEFT) {
        points->pushBack(cur);

        temp->clear();
        drawCurve(color, temp);

        return true;
    }

    if (btn == RIGHT) {
        temp->clear();
        drawCurve(color, perm);

        points->clear();
    }

    return false;
}

bool CurveTool::paintOnReleased (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    return true;
}

bool CurveTool::paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color) {
    ON_ERROR(!points, "Drawable area was null!", false);

    temp->clear();
    drawCurve(color, perm);
    points->clear();

    return false;
}

ToolManager::ToolManager() :
    current(nullptr),
    color  (MColor())   {}

ToolManager::ToolManager(Tool *_current, MColor _color) :
    current(_current),
    color  (_color)     {}

ToolManager::~ToolManager() {
    delete current;
}

void ToolManager::setTool (Tool* _tool) {
    current = _tool;
}

void ToolManager::setColor(MColor _color) {
    color = _color;
}

Tool* ToolManager::getTool() {
    return current;
}

bool ToolManager::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn) {
    return current->paintOnPressed(perm, temp, color, cur, btn);
}

bool ToolManager::paintOnMove(RenderTarget *perm, RenderTarget *temp, MPoint cur) {
    return current->paintOnMove(perm, temp, color, cur);
}

bool ToolManager::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn) {
    return current->paintOnReleased(perm, temp, color, cur, btn);
}

bool ToolManager::paintOnDeactivate(RenderTarget *perm, RenderTarget *temp) {
    return current->paintOnDeactivate(perm, temp, color);
}

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager,  FilterManager *_filtManager, Widget* _parent, MathRectangle _parentRect) :
    Widget     (_position, _size, _parent),
    manager    (_manager),
    filtManager(_filtManager),
    parentRect (_parentRect)     {
        rendTarget = new RenderTarget(_position, _size);        
        tempTarget = new RenderTarget(_position, _size);
    }

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, MathRectangle _parentRect, RenderTarget *_rendTarget) :
    Widget     (_position, _size, _parent),
    rendTarget (_rendTarget),
    manager    (_manager),
    filtManager(_filtManager),
    parentRect (_parentRect)     {
        tempTarget = new RenderTarget(_position, _size);
    }

Canvas::~Canvas() {
    delete rendTarget;
    delete tempTarget;
}

void Canvas::onScroll(MPoint shift) {
    Widget::move(shift);
}

bool Canvas::onMousePressed(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    if (parentRect.isPointInside(pos)) {
        if (filtManager && filtManager->getActive()) {
            filtManager->setRT(rendTarget);
            filtManager->applyFilter();
            
            return true;
        }

        drawing = manager->paintOnPressed(rendTarget, tempTarget, pos - this->position, btn);
    
        return drawing;
    }

    return false;
}

bool Canvas::onMouseReleased(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    if (drawing) drawing = manager->paintOnReleased(rendTarget, tempTarget, pos - this->position, btn);

    return drawing;
}

bool Canvas::onMouseMove(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    if (!isInside(pos) && drawing) {
        drawing = manager->paintOnDeactivate(rendTarget, tempTarget);
    
        return drawing;
    }

    if (!drawing) return false;

    drawing = manager->paintOnMove(rendTarget, tempTarget, pos - this->position);
    return drawing;
}

void Canvas::drawTexture(RenderTarget* toDraw, RenderTarget* drawOn) {
    ON_ERROR(!toDraw || !drawOn, "Render target was null!",);

    MImage *tmp = toDraw->getImage();

    drawOn->drawSprite(position, size, tmp, regSet);
    delete tmp;
}

void Canvas::render(RenderTarget* renderTarget) {
    ON_ERROR(!renderTarget, "Render target was null!",);

    delete regSet;

    regSet = new RegionSet();
    MathRectangle canvasRect = MathRectangle(position, size);
    regSet->addRegion(getIntersection(canvasRect, parentRect));
    
    renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL), MColor(TRANSPARENT), regSet);

    drawTexture(rendTarget, renderTarget);
    drawTexture(tempTarget, renderTarget);

    Widget::render(renderTarget);
}

void Canvas::move(MPoint shift) {
    parentRect.move(shift);
    Widget::move(shift);
}