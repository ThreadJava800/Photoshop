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
    perm->drawCircle(cur, 20, color);
}

void Brush::paintOnMove(RenderTarget *perm, MColor color, MPoint cur) {

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

bool Canvas::onMousePressed(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    manager->paintOnPressed(rendTarget, pos, btn);

    return true;
}

bool Canvas::onMouseReleased(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    manager->paintOnReleased(rendTarget, pos, btn);

    return true;
}

bool Canvas::onMouseMove(MPoint pos, MMouse btn) {
    if (!rendTarget) return false;

    manager->paintOnMove(rendTarget, pos);

    return true;
}

void Canvas::render(RenderTarget* renderTarget) {
    // renderTarget->drawRect(position, size, MColor(DEFAULT_BACK_COL), MColor(DEFAULT_BACK_COL), regSet);

    // sf::Texture tmp = rendTarget->getRenderTexture()->getTexture();
    // MImage* ttt = new MImage(&tmp);
    // renderTarget->drawSprite(position, size, ttt, regSet);
    // delete ttt;
}