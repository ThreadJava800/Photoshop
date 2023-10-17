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

void Brush::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    perm->drawCircle(cur, LINE_DIAM, color);
}

void Brush::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    perm->drawCircle(cur, LINE_DIAM, color);
}

void Brush::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {}

StraightTool::StraightTool() :
    Tool(),
    rectStart(MPoint()) {}

StraightTool::StraightTool(MPoint _start, MPoint _end) :
    Tool(_start, _end),
    rectStart(MPoint()) {}

void StraightTool::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!",);

    if (btn == LEFT) rectStart = cur;
}

void StraightTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!",);
}

void StraightTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!",);
}

CircleTool::CircleTool() :
    StraightTool()    {}

CircleTool::CircleTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end)  {}

void CircleTool::drawCircle(MPoint lu, MPoint cur, MColor color, RenderTarget *drawTarget) {
    ON_ERROR(!drawTarget, "Drawable area was null!",);

    double radius   = std::max(fabs(cur.x - lu.x) / 2, fabs(cur.y - lu.y) / 2);

    double xMin     = std::min(cur.x, lu.x);
    double yMin     = std::min(cur.y, lu.y);
    MPoint circleLU = MPoint(xMin, yMin);

    drawTarget->drawCircle(circleLU, radius, color);
}

void CircleTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!",);

    temp->clear();
    drawCircle(rectStart, cur, color, temp);
}

void CircleTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!",);

    temp->clear();
    drawCircle(rectStart, cur, color, perm);
}

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

void SquareTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "Drawable area was null!",);

    temp->clear();
    drawSquare(rectStart, cur, color, temp);
}

void SquareTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "Drawable area was null!",);

    temp->clear();
    drawSquare(rectStart, cur, color, perm);
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

void EllipseTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "Drawable area was null!",);

    temp->clear();
    drawEllipse(rectStart, cur, color, temp);
}

void EllipseTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "Drawable area was null!",);

    temp->clear();
    drawEllipse(rectStart, cur, color, perm);
}

LineTool::LineTool() :
    StraightTool()              {}

LineTool::LineTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end)  {}

void LineTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "Drawable area was null!",);

    temp->clear();
    temp->drawLine(rectStart, cur, color);
}

void LineTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "Drawable area was null!",);

    temp->clear();
    perm->drawLine(rectStart, cur, color);
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

void CurveTool::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp || !points, "Drawable area was null!",);

    if (btn == LEFT) {
        points->pushBack(cur);

        temp->clear();
        drawCurve(color, temp);
    }

    if (btn == RIGHT) {
        temp->clear();
        drawCurve(color, perm);

        points->clear();
    }
}

void CurveTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {}

void CurveTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {}

ToolManager::ToolManager() :
    current(nullptr),
    color  (MColor())   {}

ToolManager::ToolManager(Tool *_current, MColor _color) :
    current(_current),
    color  (_color)     {}

void ToolManager::setTool (Tool* _tool) {
    current = _tool;
}

void ToolManager::setColor(MColor _color) {
    color = _color;
}

void ToolManager::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn) {
    current->paintOnPressed(perm, temp, color, cur, btn);
}

void ToolManager::paintOnMove(RenderTarget *perm, RenderTarget *temp, MPoint cur) {
    current->paintOnMove(perm, temp, color, cur);
}

void ToolManager::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn) {
    current->paintOnReleased(perm, temp, color, cur, btn);
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
        manager->paintOnPressed(rendTarget, tempTarget, pos - this->position, btn);
        drawing = true;

        return true;
    }

    return false;
}

bool Canvas::onMouseReleased(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    if (drawing) manager->paintOnReleased(rendTarget, tempTarget, pos - this->position, btn);
    drawing = false;

    return true;
}

bool Canvas::onMouseMove(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    if (!drawing) return false;

    manager->paintOnMove(rendTarget, tempTarget, pos - this->position);
    return true;
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