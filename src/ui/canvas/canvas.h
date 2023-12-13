#ifndef _CANVAS_h_
#define _CANVAS_h_

#include "../../plugin.h"
#include "../../../libs/multimedia/multimedia.h"
#include "../renderable.h"
#include "filters.h"

class Tool : public plugin::ToolI {
protected:
    MPoint start = MPoint();
    MPoint last  = MPoint();

    bool is_drawing = false;

public:
    explicit Tool();
    explicit Tool(MPoint _start, MPoint _end);

    virtual ~Tool() = default;
};

class Brush : public Tool {
protected:
    List<MPoint>* points = nullptr;

    double getCatmullCoeff(double prevCoeff, MPoint p1, MPoint p2);
    void   drawCatmullOf4 (plugin::RenderTargetI* perm, plugin::RenderTargetI* temp, MColor color, MPoint p0, MPoint p1, MPoint p2, MPoint p3);
    void   drawCatmullOf3 (plugin::RenderTargetI* perm, MColor color, MPoint p1, MPoint p2, MPoint p3);
    void   drawCatmullOf2 (plugin::RenderTargetI* perm, MColor color, MPoint p1, MPoint p2);
    void   drawCatmull    (plugin::RenderTargetI* temp, plugin::RenderTargetI* perm, MColor color);
    void   copyTmpToPerm  (plugin::RenderTargetI* temp, plugin::RenderTargetI* perm, MColor color);

public:
    explicit Brush();
    explicit Brush(MPoint _start, MPoint _end);

    virtual ~Brush();

    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;

    const plugin::Texture *getIcon() const override;

    void paintOnPress  (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void disable       (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
};

class Spline : public Brush {
public:
    explicit Spline();
    explicit Spline(MPoint _start, MPoint _end);

    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;

    const plugin::Texture *getIcon() const override;

    void paintOnPress  (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void disable       (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
};

class FillTool : public Tool {
public:
    explicit FillTool();
    explicit FillTool(MPoint _start, MPoint _end);

    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;

    const plugin::Texture *getIcon() const override;

    void paintOnPress  (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void disable       (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
};

class StraightTool : public Tool {
protected:
    MPoint rectStart = MPoint();

public:
    explicit StraightTool();
    explicit StraightTool(MPoint _start, MPoint _end);

    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;

    const plugin::Texture *getIcon() const override;

    virtual ~StraightTool() = default;

    void paintOnPress  (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void disable       (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
};

class EllipseTool : public StraightTool {
private:
    void drawEllipse(MPoint lu, MPoint cur, MColor color, plugin::RenderTargetI *drawTarget);

public:
    explicit EllipseTool();
    explicit EllipseTool(MPoint _start, MPoint _end);

    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;

    const plugin::Texture *getIcon() const override;

    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
};

class SquareTool : public StraightTool {
private:
    void drawSquare(MPoint lu, MPoint cur, MColor color, plugin::RenderTargetI *drawTarget);

public:
    explicit SquareTool();
    explicit SquareTool(MPoint _start, MPoint _end);

    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;

    const plugin::Texture *getIcon() const override;

    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
};

class LineTool : public StraightTool {
public:
    explicit LineTool();
    explicit LineTool(MPoint _start, MPoint _end);

    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;

    const plugin::Texture *getIcon() const override;

    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
};

class CurveTool : public StraightTool {
private:
    List<MPoint> *points = nullptr;

    void drawCurve(MColor color, plugin::RenderTargetI *drawTarget);

public:
    explicit CurveTool();
    explicit CurveTool(MPoint _start, MPoint _end);

    ~CurveTool();

    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;

    const plugin::Texture *getIcon() const override;

    void paintOnPress  (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
    void disable       (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) override;
};

class ToolManager {
private:
    plugin::ToolI* current = nullptr;
    MColor         color   = MColor();

public:
    explicit ToolManager();
    explicit ToolManager(plugin::ToolI *_current, MColor _color);

    ~ToolManager();

    void setColor(plugin::Color color);
    void setTool (plugin::ToolI *tool);

    plugin::ToolI *getTool ();
    plugin::Color  getColor();

    void paintOnMove   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context);
    void paintOnPress  (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context);
    void paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context);
    void disableTool   (plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context);
};

class Canvas : public Widget {
private:
    bool is_drawing = false;

    RenderTarget*  rendTarget  = nullptr;
    RenderTarget*  tempTarget  = nullptr;
    ToolManager*   manager     = nullptr;
    FilterManager* filtManager = nullptr;

    MathRectangle  parentRect;

    void drawTexture(RenderTarget* toDraw, RenderTarget* drawOn);

public:
    explicit Canvas(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, MathRectangle _parentRect);
    explicit Canvas(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, MathRectangle _parentRect, RenderTarget *_rendTarget);

    ~Canvas();

    plugin::Texture* getTexture();

    void onScroll(MPoint shift);

    bool onMousePress  (plugin::MouseContext context) override;
    bool onMouseRelease(plugin::MouseContext context) override;
    bool onMouseMove   (plugin::MouseContext context) override;

    void render(RenderTarget* renderTarget) override;
    void move  (plugin::Vec2 shift)         override;

    void setTexture(plugin::Texture* texture);
};

#endif