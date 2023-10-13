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

void Brush::paintOnPressed (RenderTarget *perm, RenderTarget *tmp, MColor color, MPoint cur, MMouse btn) {
    perm->drawCircle(cur, 20, color);
    std::cout << cur.x << ' ' << cur.y << '\n';
}

void Brush::paintOnMove    (RenderTarget *perm, RenderTarget *tmp, MColor color, MPoint cur) {

}

void Brush::paintOnReleased(RenderTarget *perm, RenderTarget *tmp, MColor color, MPoint cur, MMouse btn) {

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

void ToolManager::paintOnPressed(RenderTarget *perm, RenderTarget *tmp, MPoint cur, MMouse btn) {
    current->paintOnPressed(perm, tmp, color, cur, btn);
}

void ToolManager::paintOnMove(RenderTarget *perm, RenderTarget *tmp, MPoint cur) {
    current->paintOnMove(perm, tmp, color, cur);
}

void ToolManager::paintOnReleased(RenderTarget *perm, RenderTarget *tmp, MPoint cur, MMouse btn) {
    current->paintOnReleased(perm, tmp, color, cur, btn);
}

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager) :
    Widget(_position, _size, nullptr),
    drawing   (false),
    rendTarget(nullptr),
    manager   (_manager)     {}

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager, RenderTarget *_rendTarget) :
    Widget(_position, _size, nullptr),
    drawing(false),
    rendTarget(_rendTarget),
    manager   (_manager)     {}

bool Canvas::onMousePressed(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    RenderTarget _rendTarget = RenderTarget(position, size, rendTarget->getRenderWindow());
    manager->paintOnPressed(rendTarget, &_rendTarget, pos, btn);

    return false;
}

bool Canvas::onMouseReleased(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;
    RenderTarget _rendTarget = RenderTarget(position, size, rendTarget->getRenderWindow());
    manager->paintOnReleased(rendTarget, &_rendTarget, pos, btn);

    return false;
}

bool Canvas::onMouseMove(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;
    RenderTarget _rendTarget = RenderTarget(position, size, rendTarget->getRenderWindow());
    manager->paintOnMove(rendTarget, &_rendTarget, pos);

    return false;
}

void Canvas::render(RenderTarget* renderTarget) {
    sf::Texture tmp = rendTarget->getRenderTexture()->getTexture();
    // MImage ttt = MImage(&tmp);
    // renderTarget->drawSprite(position, size, &ttt);
}