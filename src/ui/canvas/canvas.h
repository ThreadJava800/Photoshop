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

    virtual bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) = 0;
    virtual bool paintOnMove      (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             = 0;
    virtual bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) = 0;
    virtual bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp)                                       = 0;
};

class Brush : public Tool {
public:
    explicit Brush();
    explicit Brush(MPoint _start, MPoint _end);

    bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnMove      (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp)                                       override;
};

class StraightTool : public Tool {
protected:
    MPoint rectStart = MPoint();

public:
    explicit StraightTool();
    explicit StraightTool(MPoint _start, MPoint _end);

    bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnMove      (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp)                                       override;
};

class CircleTool : public StraightTool {
private:
    void drawCircle(MPoint lu, MPoint cur, MColor color, RenderTarget *drawTarget);

public:
    explicit CircleTool();
    explicit CircleTool(MPoint _start, MPoint _end);

    bool paintOnMove    (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    bool paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
};

class EllipseTool : public StraightTool {
private:
    void drawEllipse(MPoint lu, MPoint cur, MColor color, RenderTarget *drawTarget);

public:
    explicit EllipseTool();
    explicit EllipseTool(MPoint _start, MPoint _end);

    bool paintOnMove    (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    bool paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
};

class SquareTool : public StraightTool {
private:
    void drawSquare(MPoint lu, MPoint cur, MColor color, RenderTarget *drawTarget);

public:
    explicit SquareTool();
    explicit SquareTool(MPoint _start, MPoint _end);

    bool paintOnMove    (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    bool paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
};

class LineTool : public StraightTool {
public:
    explicit LineTool();
    explicit LineTool(MPoint _start, MPoint _end);

    bool paintOnMove    (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    bool paintOnReleased(RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
};

class CurveTool : public StraightTool {
private:
    List<MPoint> *points = nullptr;

    void drawCurve(MColor color, RenderTarget *drawTarget);

public:
    explicit CurveTool();
    explicit CurveTool(MPoint _start, MPoint _end);

    ~CurveTool();

    bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp)                                       override;
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

    bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn);
    bool paintOnMove      (RenderTarget *perm, RenderTarget *temp, MPoint cur);
    bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn);
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp);
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