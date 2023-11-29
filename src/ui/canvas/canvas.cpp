#include "canvas.h"

Tool::Tool() :
    start(MPoint()),
    last (MPoint())     {}

Tool::Tool(MPoint _start, MPoint _end) :
    start(_start),
    last (_end)         {}

Brush::Brush() :
    Tool()  {
        points = new List<MPoint>();
    }

Brush::Brush(MPoint _start, MPoint _end) :
    Tool(_start, _end)  {
        points = new List<MPoint>();
    }

Brush::~Brush() {
    delete points;
}

double Brush::getCatmullCoeff(double prevCoeff, MPoint p1, MPoint p2) {
    MPoint diffPoint = p2 - p1;

    return std::pow((diffPoint | diffPoint), CATMULL_ALPHA * 0.5) + prevCoeff;
}

List<MPoint>* Brush::getCatmullCoeffs(MPoint p0, MPoint p1, MPoint p2, MPoint p3, bool setOf3) {
    ON_ERROR(!points, "Point list ptr area was nullptr!", nullptr);

    double t0 = 0;
    double t1 = getCatmullCoeff(t0, p0, p1);
    double t2 = getCatmullCoeff(t1, p1, p2);
    double t3 = getCatmullCoeff(t2, p2, p3);

    List<MPoint>* coeffs = new List<MPoint>();

    for (double i = 0; i < 1; i += 0.01) {
        double t  = lerp(t1, t2, i);

        if (t1 == t0 || t1 == t3 || t1 == t2 || t2 == t3 || t2 == t0) continue;

        MPoint a1 = p0 * ((t1 - t) / (t1 - t0)) + p1 * ((t - t0) / (t1 - t0));
        MPoint a2 = p1 * ((t2 - t) / (t2 - t1)) + p2 * ((t - t1) / (t2 - t1));
        MPoint a3 = p2 * ((t3 - t) / (t3 - t2)) + p3 * ((t - t2) / (t3 - t2));
        MPoint b1 = a1 * ((t2 - t) / (t2 - t0)) + a2 * ((t - t0) / (t2 - t0));
        if (setOf3) {
            coeffs->pushBack(b1);
            continue;
        }

        MPoint b2 = a2 * ((t3 - t) / (t3 - t1)) + a3 * ((t - t1) / (t3 - t1));
        MPoint c  = b1 * ((t2 - t) / (t2 - t1)) + b2 * ((t - t1) / (t2 - t1));

        coeffs->pushBack(c);
    }

    return coeffs;
}

void Brush::drawCatmullOf3(plugin::RenderTargetI* perm, MColor color, MPoint p1, MPoint p2, MPoint p3) {
    ON_ERROR(!perm, "RenderTarget was null!",);

    List<MPoint>* drawPoints = getCatmullCoeffs(p1, p2, p3, MPoint(), true);
    size_t drawCnt = drawPoints->getSize();
        
    for (long i = 0; i < long(drawCnt); i++) {
        perm->drawEllipse((*drawPoints)[i].toVec2(), {LINE_DIAM, LINE_DIAM}, color.toPlColor());
    }

    delete drawPoints;
}

void Brush::drawCatmull(plugin::RenderTargetI* perm, MColor color) {
    ON_ERROR(!perm, "RenderTarget was null!",);

    size_t pointCnt = points->getSize();

    if (pointCnt == 1) {
        perm->drawEllipse((*points)[0].toVec2(), {LINE_DIAM, LINE_DIAM}, color.toPlColor());
    }
    if (pointCnt == 2) {
        perm->drawLine((*points)[0].toVec2(), (*points)[1].toVec2(), color.toPlColor());
    }
    if (pointCnt == 3) {
        drawCatmullOf3(perm, color, (*points)[2], (*points)[1], (*points)[0]);
        drawCatmullOf3(perm, color, (*points)[0], (*points)[1], (*points)[2]);
    }

    if (pointCnt < 4) return;

    drawCatmullOf3(perm, color, (*points)[2], (*points)[1], (*points)[0]);
    drawCatmullOf3(perm, color, (*points)[pointCnt - 3], (*points)[pointCnt - 2], (*points)[pointCnt - 1]);
}

plugin::Array<const char *> Brush::getParamNames() {
    return {};
}

plugin::Array<double> Brush::getParams() {
    return {};
}

void Brush::setParams(plugin::Array<double> params) {

}

const plugin::Texture *Brush::getIcon() {
    return nullptr;
}

void Brush::paintOnPress(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "RenderTarget was null!",);

    is_drawing = true;

    if (points->getSize() > 4) points->popFront();

    points->pushBack(MPoint(context.position));
    drawCatmull(data, MColor(color));
}

void Brush::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    points->clear();
    is_drawing = false;
}

void Brush::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "RenderTarget was null!",);

    if (!is_drawing) return;

    if (points->getSize() > 4) points->popFront();

    points->pushBack(MPoint(context.position));
    drawCatmull(data, MColor(color));
}

void Brush::disable(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    points->clear();
    is_drawing = false;
}

Spline::Spline() :
    Brush()             {}

Spline::Spline(MPoint _start, MPoint _end) :
    Brush(_start, _end) {}

plugin::Array<const char *> Spline::getParamNames() {
    return {};
}

plugin::Array<double> Spline::getParams() {
    return {};
}

void Spline::setParams(plugin::Array<double> params) {

}

const plugin::Texture *Spline::getIcon() {
    return nullptr;
}

void Spline::paintOnPress(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "RenderTarget was null!",);

    is_drawing = true;

    if (context.button == plugin::MouseButton::Left) {
        points->pushBack(MPoint(context.position));

        tmp->clear();
        drawCatmull(tmp,  MColor(color));

        if (points->getSize() > 3) {
            MPoint notDrawPoint = points->pop();
            drawCatmull(data, MColor(color));

            points->pushBack(notDrawPoint);
        }

        return;
    }

    disable(data, tmp, context, color);
}

void Spline::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {

}

void Spline::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {

}

void Spline::disable(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "RenderTarget was null!",);

    tmp->clear();
    drawCatmull(data, MColor(color));

    points->clear();
}

FillTool::FillTool() :
    Tool()              {}

FillTool::FillTool(MPoint _start, MPoint _end) :
    Tool(_start, _end)  {}

plugin::Array<const char *> FillTool::getParamNames() {
    return {};
}

plugin::Array<double> FillTool::getParams() {
    return {};
}

void FillTool::setParams(plugin::Array<double> params) {

}

const plugin::Texture *FillTool::getIcon() {
    return nullptr;
}

void FillTool::paintOnPress(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "RenderTarget was null!",);

    MImage textureImage               = MImage(data->getTexture());
    List<List<MColor>*>* pixelListPtr = textureImage.getPixels();
    if (!pixelListPtr) return;

    size_t xSize = CANVAS_SIZE;
    size_t ySize = CANVAS_SIZE;

    List<MPoint>  bfsList = List<MPoint>();
    List<bool>    was     = List<bool>(xSize * ySize);
    bfsList.pushBack(MPoint(context.position));

    List<List<MColor>*> resPixels = List<List<MColor>*>(xSize);
    for (size_t i = 0; i < xSize; i++) {
        resPixels.pushBack(new List<MColor>(ySize));
        for (size_t j = 0; j < ySize; j++) {
            resPixels[i]->pushBack(MColor());
        }
    }

    while (bfsList.getSize()) {
        MPoint bfs = bfsList.pop();

        bfs.x = int(bfs.x);
        bfs.y = int(bfs.y);

        if (was[bfs.y * xSize + bfs.x]) continue;
        was[bfs.y * xSize + bfs.x] = true;

        if (bfs.y != 0) {
            MPoint up = MPoint(bfs.x, bfs.y - 1);
            if ((*((*pixelListPtr)[up.x]))[up.y] == (*((*pixelListPtr)[bfs.x]))[bfs.y]) {
                bfsList.pushBack(up);

                (*(resPixels)[up.x])[up.y] = MColor(color);
            }
        }

        if (bfs.x != 0) {
            MPoint lp = MPoint(bfs.x - 1, bfs.y);
            if ((*((*pixelListPtr)[lp.x]))[lp.y] == (*((*pixelListPtr)[bfs.x]))[bfs.y]) {
                bfsList.pushBack(lp);

                (*(resPixels)[lp.x])[lp.y] = MColor(color);
            }
        }

        if (bfs.x != xSize - 1) {
            MPoint rp = MPoint(bfs.x + 1, bfs.y);
            if ((*((*pixelListPtr)[rp.x]))[rp.y] == (*((*pixelListPtr)[bfs.x]))[bfs.y]) {
                bfsList.pushBack(rp);

                (*(resPixels)[rp.x])[rp.y] = MColor(color);
            }
        }

        if (bfs.y != ySize - 1) {
            MPoint bp = MPoint(bfs.x, bfs.y + 1);
            if ((*((*pixelListPtr)[bp.x]))[bp.y] == (*((*pixelListPtr)[bfs.x]))[bfs.y]) {
                bfsList.pushBack(bp);

                (*(resPixels)[bp.x])[bp.y] = MColor(color);
            }
        }
    }

    textureImage.imgFromPixel(&resPixels);
    data->drawTexture({0, 0}, {(double)xSize, (double)ySize}, textureImage.toPluginTexture());

    for (size_t i = 0; i < xSize; i++) delete (*pixelListPtr)[i];
    delete pixelListPtr;

    for (size_t i = 0; i < xSize; i++) delete resPixels[i];
}

void FillTool::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {

}

void FillTool::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {

}

void FillTool::disable(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {

}

StraightTool::StraightTool() :
    Tool(),
    rectStart(MPoint()) {}

StraightTool::StraightTool(MPoint _start, MPoint _end) :
    Tool(_start, _end),
    rectStart(MPoint()) {}

plugin::Array<const char *> StraightTool::getParamNames() {
    return {};
}

plugin::Array<double> StraightTool::getParams() {
    return {};
}

void StraightTool::setParams(plugin::Array<double> params) {

}

const plugin::Texture *StraightTool::getIcon() {
    return nullptr;
}

void StraightTool::paintOnPress(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "RenderTarget was null!",);

    is_drawing = true;
    if (context.button == plugin::MouseButton::Left) rectStart = MPoint(context.position);
}

void StraightTool::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {

}

void StraightTool::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {

}

void StraightTool::disable(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {

}

SquareTool::SquareTool() :
    StraightTool()             {}

SquareTool::SquareTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end) {}

plugin::Array<const char *> SquareTool::getParamNames() {
    return {};
}

plugin::Array<double> SquareTool::getParams() {
    return {};
}

void SquareTool::setParams(plugin::Array<double> params) {

}

const plugin::Texture *SquareTool::getIcon() {
    return nullptr;
}

void SquareTool::drawSquare(MPoint lu, MPoint cur, MColor color, plugin::RenderTargetI *drawTarget) {
    ON_ERROR(!drawTarget, "Drawable area was null!",);

    MPoint size = MPoint(fabs(cur.x - lu.x), fabs(cur.y - lu.y));

    double xMin   = std::min(cur.x, lu.x);
    double yMin   = std::min(cur.y, lu.y);
    MPoint rectLU = MPoint(xMin, yMin);

    drawTarget->drawRect(rectLU.toVec2(), size.toVec2(), color.toPlColor());
}

void SquareTool::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "Drawable area was null!",);

    tmp->clear();
    drawSquare(rectStart, MPoint(context.position), MColor(color), data);
    is_drawing = false;
}

void SquareTool::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "Drawable area was null!",);

    if (!is_drawing) return;

    tmp->clear();
    drawSquare(rectStart, MPoint(context.position), MColor(color), tmp);
}

EllipseTool::EllipseTool() :
    StraightTool()              {}

EllipseTool::EllipseTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end)  {}

plugin::Array<const char *> EllipseTool::getParamNames() {
    return {};
}

plugin::Array<double> EllipseTool::getParams() {
    return {};
}

void EllipseTool::setParams(plugin::Array<double> params) {

}

const plugin::Texture *EllipseTool::getIcon() {
    return nullptr;
}

void EllipseTool::drawEllipse(MPoint lu, MPoint cur, MColor color, plugin::RenderTargetI *drawTarget) {
    ON_ERROR(!drawTarget, "Drawable area was null!",);

    double xMin     = std::min(cur.x, lu.x);
    double yMin     = std::min(cur.y, lu.y);
    MPoint circleLU = MPoint(xMin, yMin);

    double height = fabs(cur.y - lu.y);
    double length = fabs(cur.x - lu.x);

    if (height < EPSILON || length < EPSILON) return;

    drawTarget->drawEllipse(circleLU.toVec2(), {length, height}, color.toPlColor());
}

void EllipseTool::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "Drawable area was null!",);

    tmp->clear();
    drawEllipse(rectStart, MPoint(context.position), MColor(color), data);
    is_drawing = false;
}

void EllipseTool::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "Drawable area was null!",);

    if (!is_drawing) return;

    tmp->clear();
    drawEllipse(rectStart, MPoint(context.position), MColor(color), tmp);
}

LineTool::LineTool() :
    StraightTool()              {}

LineTool::LineTool(MPoint _start, MPoint _end) :
    StraightTool(_start, _end)  {}

plugin::Array<const char *> LineTool::getParamNames() {
    return {};
}

plugin::Array<double> LineTool::getParams() {
    return {};
}

void LineTool::setParams(plugin::Array<double> params) {

}

const plugin::Texture *LineTool::getIcon() {
    return nullptr;
}

void LineTool::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "Drawable area was null!",);

    tmp->clear();
    data->drawLine(rectStart.toVec2(), context.position, color);
    is_drawing = false;
}

void LineTool::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp, "Drawable area was null!",);

    if (!is_drawing) return;

    tmp->clear();
    tmp->drawLine(rectStart.toVec2(), context.position, color);
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

void CurveTool::drawCurve(MColor color, plugin::RenderTargetI *drawTarget) {
    ON_ERROR(!drawTarget || !points, "Drawable area was null!",);

    size_t listSize = points->getSize();

    if (listSize == 1) {
        drawTarget->setPixel((*points)[0].toVec2(), color.toPlColor());
        return;
    }

    for (size_t i = 1; i < listSize; i++) {
        drawTarget->drawLine((*points)[i - 1].toVec2(), (*points)[i].toVec2(), color.toPlColor());
    }
}

plugin::Array<const char *> CurveTool::getParamNames() {
    return {};
}

plugin::Array<double> CurveTool::getParams() {
    return {};
}

void CurveTool::setParams(plugin::Array<double> params) {

}

const plugin::Texture *CurveTool::getIcon() {
    return nullptr;
}

void CurveTool::paintOnPress(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!data || !tmp || !points, "Drawable area was null!",);

    if (context.button == plugin::MouseButton::Left) {
        points->pushBack(MPoint(context.position));

        tmp->clear();
        drawCurve(MColor(color), tmp);

        is_drawing = true;
    }

    if (context.button == plugin::MouseButton::Right) {
        tmp->clear();
        drawCurve(MColor(color), data);

        points->clear();

        is_drawing = false;
    }
}

void CurveTool::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {

}

void CurveTool::disable(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context, plugin::Color color) {
    ON_ERROR(!points, "Drawable area was null!",);

    tmp->clear();
    drawCurve(MColor(color), data);
    points->clear();
    is_drawing = false;
}

ToolManager::ToolManager() :
    current(nullptr),
    color  (MColor())   {}

ToolManager::ToolManager(plugin::ToolI *_current, MColor _color) :
    current(_current),
    color  (_color)     {}

ToolManager::~ToolManager() {
    // delete current;
}

void ToolManager::setColor(plugin::Color color) {
    this->color = MColor(color);
}

void ToolManager::setTool(plugin::ToolI *tool) {
    current = tool;
}

plugin::ToolI* ToolManager::getTool() {
    return current;
}

plugin::Color ToolManager::getColor() {
    return color.toPlColor();
}

void ToolManager::paintOnMove(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context) {
    current->paintOnMove(data, tmp, context, color.toPlColor());
}

void ToolManager::paintOnPress(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context) {
    current->paintOnPress(data, tmp, context, color.toPlColor());
}

void ToolManager::paintOnRelease(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context) {
    current->paintOnRelease(data, tmp, context, color.toPlColor());
}

void ToolManager::disableTool(plugin::RenderTargetI *data, plugin::RenderTargetI *tmp, plugin::MouseContext context) {
    current->disable(data, tmp, context, color.toPlColor());
}

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager,  FilterManager *_filtManager, Widget* _parent, MathRectangle _parentRect) :
    Widget     (_position, _size, _parent),
    manager    (_manager),
    filtManager(_filtManager),
    parentRect (_parentRect)     {
        rendTarget = new RenderTarget(_position, _size);        
        tempTarget = new RenderTarget(_position, _size);
    }

Canvas::Canvas(MPoint _position, MPoint _size, ToolManager *_manager, FilterManager *_filtManager, Widget* _parent, MathRectangle _parentRect, RenderTarget *_rendTarget) :
    Widget     (_position, _size, _parent),
    rendTarget (_rendTarget),
    manager    (_manager),
    filtManager(_filtManager),
    parentRect (_parentRect)     {
        tempTarget = new RenderTarget(_position, _size);
    }

Canvas::~Canvas() {
    delete rendTarget;
    delete tempTarget;
}

void Canvas::onScroll(MPoint shift) {
    Widget::move(shift.toVec2());
}

plugin::Texture* Canvas::getTexture() {
    return rendTarget->getTexture();
}

bool Canvas::onMousePress(plugin::MouseContext context) {
    if (!rendTarget) return false;

    MPoint pos            = MPoint(context.position);
    plugin::Vec2 draw_pos = {context.position.x - this->position.x, context.position.y - this->position.y};

    if (parentRect.isPointInside(MPoint(pos))) {
        if (filtManager && filtManager->getActive()) {
            filtManager->setRenderTarget(rendTarget);
            filtManager->applyFilter();
            
            return true;
        }

        is_drawing = true;

        manager->paintOnPress(rendTarget, tempTarget, {draw_pos, context.button});
        return true;
    } else {
        is_drawing = false;
        manager->disableTool(rendTarget, tempTarget, {draw_pos, context.button});
    }

    return false;
}

bool Canvas::onMouseRelease(plugin::MouseContext context) {
    if (!rendTarget || !is_drawing) return false;

    MPoint pos = MPoint(context.position);

    plugin::Vec2 draw_pos = {context.position.x - this->position.x, context.position.y - this->position.y};
    manager->paintOnRelease(rendTarget, tempTarget, {draw_pos, context.button});

    is_drawing = false;

    return true;
}

bool Canvas::onMouseMove(plugin::MouseContext context) {
    if (!rendTarget) return false;

    MPoint pos = MPoint(context.position);

    if (!is_drawing) return false;

    if (!isInside(pos)) {
        if (is_drawing) manager->disableTool(rendTarget, tempTarget, context);
        is_drawing = false;

        return true;
    }


    plugin::Vec2 draw_pos = {context.position.x - this->position.x, context.position.y - this->position.y};
    manager->paintOnMove(rendTarget, tempTarget, {draw_pos, context.button});
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

    delete regSet;

    regSet = new RegionSet();
    MathRectangle canvasRect = MathRectangle(position, size);
    regSet->addRegion(getIntersection(canvasRect, parentRect));
    
    renderTarget->drawRect(position, size, DEFAULT_BACK_COL, MColor::TRANSPARENT, regSet);

    drawTexture(rendTarget, renderTarget);
    drawTexture(tempTarget, renderTarget);

    Widget::render(renderTarget);
}

void Canvas::move(plugin::Vec2 shift) {
    parentRect.move(MPoint(shift));
    Widget::move(shift);
}