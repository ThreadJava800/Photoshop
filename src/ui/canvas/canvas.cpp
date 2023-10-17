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

void StraightTool::drawCircle(MPoint lu, MPoint cur, MColor color, RenderTarget *drawTarget) {
    ON_ERROR(!drawTarget, "Drawable area was null!",);

    MPoint circleLu = MPoint(std::min(lu.x, cur.x), std::min(lu.y, cur.y));

    double radius   = (cur - lu).getLen() / 2;
    MPoint centre   = MPoint(circleLu.x + radius, circleLu.y + radius);

    drawTarget->drawCircle(centre, radius, color);
}

void StraightTool::paintOnPressed(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!",);

    if (btn == LEFT) rectStart = cur;
}

void StraightTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!",);
}

void StraightTool::paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!",);

    temp->clear();
    drawCircle(rectStart, cur, color, perm);
}

CircleTool::CircleTool() :
    StraightTool()    {}

CircleTool::CircleTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end)  {}

void CircleTool::paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) {
    ON_ERROR(!perm || !temp, "RenderTarget was null!",);

    temp->clear();
    drawCircle(rectStart, cur, color, temp);
}

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