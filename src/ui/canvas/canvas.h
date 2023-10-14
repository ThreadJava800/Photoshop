#ifndef _CANVAS_h_
#define _CANVAS_h_

#include "../../../libs/multimedia/multimedia.h"
#include "../renderable.h"

class Tool {
protected:
    MPoint start = MPoint();
    MPoint last  = MPoint();

public:
    explicit Tool();
    explicit Tool(MPoint _start, MPoint _end);

    virtual void paintOnPressed (RenderTarget *perm, MColor color, MPoint cur, MMouse btn) = 0;
    virtual void paintOnMove    (RenderTarget *perm, MColor color, MPoint cur)             = 0;
    virtual void paintOnReleased(RenderTarget *perm, MColor color, MPoint cur, MMouse btn) = 0;
};

class Brush : public Tool {
public:
    explicit Brush();
    explicit Brush(MPoint _start, MPoint _end);

    void paintOnPressed (RenderTarget *perm, MColor color, MPoint cur, MMouse btn) override;
    void paintOnMove    (RenderTarget *perm, MColor color, MPoint cur)             override;
    void paintOnReleased(RenderTarget *perm, MColor color, MPoint cur, MMouse btn) override;
};

class ToolManager {
private:
    Tool*  current = nullptr;
    MColor color   = MColor();

public:
    explicit ToolManager();
    explicit ToolManager(Tool *_current, MColor _color);

    void setTool (Tool* _tool);
    void setColor(MColor _color);

    void paintOnPressed (RenderTarget *perm, MPoint cur, MMouse btn);
    void paintOnMove    (RenderTarget *perm, MPoint cur);
    void paintOnReleased(RenderTarget *perm, MPoint cur, MMouse btn);
};

class Canvas : public Widget {
private:
    bool          drawing    = false;
    RenderTarget* rendTarget = nullptr;
    ToolManager * manager    = nullptr;

public:
    explicit Canvas(MPoint _position, MPoint _size, ToolManager *_manager);
    explicit Canvas(MPoint _position, MPoint _size, ToolManager *_manager, RenderTarget *_rendTarget);

    bool onMousePressed (MPoint pos, MMouse btn) override;
    bool onMouseReleased(MPoint pos, MMouse btn) override;
    bool onMouseMove    (MPoint pos, MMouse btn) override;

    void render(RenderTarget* renderTarget) override;
};

#endif