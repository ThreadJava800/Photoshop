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

    virtual void paintOnPressed (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) = 0;
    virtual void paintOnMove    (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             = 0;
    virtual void paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) = 0;
};

class Brush : public Tool {
public:
    explicit Brush();
    explicit Brush(MPoint _start, MPoint _end);

    void paintOnPressed (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    void paintOnMove    (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    void paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
};

class StraightTool : public Tool {
protected:
    MPoint rectStart = MPoint();

    void drawCircle(MPoint lu, MPoint cur, MColor color, RenderTarget *drawTarget);

public:
    explicit StraightTool();
    explicit StraightTool(MPoint _start, MPoint _end);

    void paintOnPressed (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    void paintOnMove    (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    void paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
};

class CircleTool : public StraightTool {
public:
    explicit CircleTool();
    explicit CircleTool(MPoint _start, MPoint _end);

    void paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) override;
};

class EllipseTool : StraightTool {
    explicit EllipseTool();
    explicit EllipseTool(MPoint _start, MPoint _end);

    void paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) override;
};

class SquareTool : StraightTool {
    explicit SquareTool();
    explicit SquareTool(MPoint _start, MPoint _end);

    void paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) override;
};

class LineTool : StraightTool {
    explicit LineTool();
    explicit LineTool(MPoint _start, MPoint _end);

    void paintOnMove(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur) override;
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

    void paintOnPressed (RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn);
    void paintOnMove    (RenderTarget *perm, RenderTarget *temp, MPoint cur);
    void paintOnReleased(RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn);
};

class Canvas : public Widget {
private:
    bool          drawing    = false;
    RenderTarget* rendTarget = nullptr;
    RenderTarget* tempTarget = nullptr;
    ToolManager * manager    = nullptr;

    void drawTexture(RenderTarget* toDraw, RenderTarget* drawOn);

public:
    explicit Canvas(MPoint _position, MPoint _size, ToolManager *_manager);
    explicit Canvas(MPoint _position, MPoint _size, ToolManager *_manager, RenderTarget *_rendTarget);

    ~Canvas();

    bool onMousePressed (MPoint pos, MMouse btn) override;
    bool onMouseReleased(MPoint pos, MMouse btn) override;
    bool onMouseMove    (MPoint pos, MMouse btn) override;

    void render(RenderTarget* renderTarget) override;
};

#endif