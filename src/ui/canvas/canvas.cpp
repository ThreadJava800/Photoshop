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

void Brush::paintOnPressed(RenderTarget *perm, MColor color, MPoint cur, MMouse btn) {
    perm->drawCircle(cur, LINE_DIAM, color);
}

void Brush::paintOnMove(RenderTarget *perm, MColor color, MPoint cur) {
    perm->drawCircle(cur, LINE_DIAM, color);
}

void Brush::paintOnReleased(RenderTarget *perm, MColor color, MPoint cur, MMouse btn) {

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

void ToolManager::paintOnPressed(RenderTarget *perm, MPoint cur, MMouse btn) {
    current->paintOnPressed(perm, color, cur, btn);
}

void ToolManager::paintOnMove(RenderTarget *perm, MPoint cur) {
    current->paintOnMove(perm, color, cur);
}

void ToolManager::paintOnReleased(RenderTarget *perm, MPoint cur, MMouse btn) {
    current->paintOnReleased(perm, color, cur, btn);
}

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager) :
    Widget (_position, _size, nullptr),
    drawing(false),
    manager(_manager)     {
        rendTarget = new RenderTarget(_position, _size);
    }

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager, RenderTarget *_rendTarget) :
    Widget    (_position, _size, nullptr),
    drawing   (false),
    rendTarget(_rendTarget),
    manager   (_manager)     {}

Canvas::~Canvas() {
    delete rendTarget;
}

bool Canvas::onMousePressed(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    MathRectangle canvRect = MathRectangle(position, size);
    if (canvRect.isPointInside(pos)) {
        manager->paintOnPressed(rendTarget, pos - this->position, btn);
        drawing = true;
        return true;
    }

    return false;
}

bool Canvas::onMouseReleased(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    manager->paintOnReleased(rendTarget, pos - this->position, btn);
    drawing = false;

    return false;
}

bool Canvas::onMouseMove(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    if (drawing) {
        manager->paintOnMove(rendTarget, pos - this->position);
        return true;
    }

    return false;
}

void Canvas::render(RenderTarget* renderTarget) {
    renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL), MColor(TRANSPARENT), regSet);
    renderTarget->drawFrame(position, size, MColor(BLACK));

    sf::Texture* tmp =  new sf::Texture();
    *tmp = rendTarget->getRenderTexture()->getTexture();

    MImage* textureWrapper = new MImage(tmp);
    renderTarget->drawSprite(position, size, textureWrapper, regSet);
    delete textureWrapper;

    Widget::render(renderTarget);
}