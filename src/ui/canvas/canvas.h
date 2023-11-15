#ifndef _CANVAS_h_
#define _CANVAS_h_

#include "../../../libs/multimedia/multimedia.h"
#include "../renderable.h"
#include "filters.h"

class Tool {
protected:
    MPoint start = MPoint();
    MPoint last  = MPoint();

public:
    explicit Tool();
    explicit Tool(MPoint _start, MPoint _end);

    virtual ~Tool() = default;

    virtual bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) = 0;
    virtual bool paintOnMove      (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             = 0;
    virtual bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) = 0;
    virtual bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color)                                       = 0;
};

class Brush : public Tool {
protected:
    List<MPoint>* points = nullptr;

    double        getCatmullCoeff (double prevCoeff, MPoint p1, MPoint p2);
    List<MPoint>* getCatmullCoeffs(MPoint p0, MPoint p1, MPoint p2, MPoint p3, bool setOf3 = false);
    void          drawCatmullOf3  (RenderTarget* perm, MColor color, MPoint p1, MPoint p2, MPoint p3);
    void          drawCatmull     (RenderTarget* perm, MColor color);

public:
    explicit Brush();
    explicit Brush(MPoint _start, MPoint _end);

    ~Brush();

    bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnMove      (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color)                         override;
};

class Spline : public Brush {
public:
    explicit Spline();
    explicit Spline(MPoint _start, MPoint _end);

    bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnMove      (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color)                         override;
};

class FillTool : public Tool {
public:
    explicit FillTool();
    explicit FillTool(MPoint _start, MPoint _end);

    bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnMove      (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur)             override;
    bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MColor color, MPoint cur, MMouse btn) override;
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color)                         override;
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
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color)                                       override;
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
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp, MColor color)                                       override;
};

class ToolManager {
private:
    Tool*  current = nullptr;
    MColor color   = MColor();

public:
    explicit ToolManager();
    explicit ToolManager(Tool *_current, MColor _color);

    ~ToolManager();

    void setTool (Tool* _tool);
    void setColor(MColor _color);

    Tool* getTool();

    bool paintOnPressed   (RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn);
    bool paintOnMove      (RenderTarget *perm, RenderTarget *temp, MPoint cur);
    bool paintOnReleased  (RenderTarget *perm, RenderTarget *temp, MPoint cur, MMouse btn);
    bool paintOnDeactivate(RenderTarget *perm, RenderTarget *temp);
};

class Canvas : public Widget {
private:
    bool           drawing     = false;
    RenderTarget * rendTarget  = nullptr;
    RenderTarget * tempTarget  = nullptr;
    ToolManager  * manager     = nullptr;
    FilterManager* filtManager = nullptr;

    MathRectangle  parentRect;

    void drawTexture(RenderTarget* toDraw, RenderTarget* drawOn);

public:
    explicit Canvas(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, MathRectangle _parentRect);
    explicit Canvas(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, MathRectangle _parentRect, RenderTarget *_rendTarget);

    ~Canvas();

    MImage* getTexture();

    void onScroll(MPoint shift);

    bool onMousePress  (plugin::MouseContext context) override;
    bool onMouseRelease(plugin::MouseContext context) override;
    bool onMouseMove   (plugin::MouseContext context) override;

    void render(RenderTarget* renderTarget) override;
    void move  (MPoint shift)               override;
};

#endif