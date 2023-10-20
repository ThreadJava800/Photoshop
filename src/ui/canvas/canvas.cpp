#include "canvas.h"

Tool::Tool() :
    start(MPoint()),
    last (MPoint())     {}

Tool::Tool(MPoint _start, MPoint _end) :
    start(_start),
    last (_end)         {}

Brush::Brush() :
    Tool()  {}

Brush::Brush(MPoint _start, MPoint _end) :
    Tool(_start, _end)  {}

bool Brush::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    perm->drawCircle(cur, LINE_DIAM, color);
    return true;
}

bool Brush::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!", false);

    perm->drawCircle(cur, LINE_DIAM, color);
    return true;
}

bool Brush::paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color) { return false; }

bool Brush::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) { return false; }

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

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager) :
    Widget (_position, _size, nullptr),
    manager(_manager)  {
        rendTarget = new RenderTarget(_position, _size);        
        tempTarget = new RenderTarget(_position, _size);
    }

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager, RenderTarget *_rendTarget) :
    Widget    (_position, _size, nullptr),
    rendTarget(_rendTarget),
    manager   (_manager)     {
        tempTarget = new RenderTarget(_position, _size);
    }

Canvas::~Canvas() {
    delete rendTarget;
    delete tempTarget;
}

bool Canvas::onMousePressed(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    MathRectangle canvRect = MathRectangle(position, size);
    if (canvRect.isPointInside(pos)) {
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

    MathRectangle canvRect = MathRectangle(position, size);
    if (!canvRect.isPointInside(pos) && drawing) {
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

    renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL), MColor(TRANSPARENT), regSet);
    renderTarget->drawFrame(position, size, MColor(sf::Color::Red), regSet);

    drawTexture(rendTarget, renderTarget);
    drawTexture(tempTarget, renderTarget);

    Widget::render(renderTarget);
}