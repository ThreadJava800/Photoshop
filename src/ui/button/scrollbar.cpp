#include "scrollbar.h"

ScrollBar::ScrollBar(MPoint _position, MPoint _size, MPoint _sliderPos, MPoint _sliderSize, Widget* _parent, ScrollFunc _func, void* _args, MPoint _delta) :
    Button    (_position, _size, MColor(TRANSPARENT), _parent),
    sliderPos (_sliderPos),
    sliderSize(_sliderSize),
    isMoving  (false),
    onScroll  (_func),
    scrollArgs(_args),
    delta     (_delta)    {}

bool ScrollBar::onMousePressed(MPoint pos, MMouse btn) {
    if (!isInside(pos)) return false;

    MathRectangle sliderRect = MathRectangle(sliderPos, sliderSize);
    if (sliderRect.isPointInside(pos)) {
        isMoving = true;
        prevPos  = pos;
        return true;
    }

    return false;
}

bool ScrollBar::onMouseMove(MPoint pos, MMouse btn) {
    if (isMoving) {
        if (onScroll) onScroll(scrollArgs, MPoint((pos.x - prevPos.x) * delta.x, (pos.y - prevPos.y) * delta.y));
        
        if (delta.x != 0) sliderPos.x += (pos.x - prevPos.x);
        if (delta.y != 0) sliderPos.y += (pos.y - prevPos.y);

        prevPos = pos;

        return true;
    }

    return false;
}

bool ScrollBar::onMouseReleased(MPoint pos, MMouse btn) {
    isMoving = false;
    return true;
}

void ScrollBar::render(RenderTarget* renderTarget) {
    // drawing area
    renderTarget->drawRect(position, size, MColor(GRAY), MColor(BLACK), regSet);
    // drawing slider
    renderTarget->drawRect(sliderPos, sliderSize, MColor(BLACK), MColor(BLACK), regSet);
}

void ScrollBar::move(MPoint shift) {
    sliderPos += shift;
    Widget::move(shift);
}