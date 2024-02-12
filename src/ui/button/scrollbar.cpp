#include "scrollbar.h"

ScrollBar::ScrollBar(MPoint _position, MPoint _size, MPoint _sliderPos, MPoint _sliderSize, Widget* _parent, ScrollFunc _func, void* _args, MPoint _delta) :
    Button    (_position, _size, MColor::TRANSPARENT, _parent),
    sliderPos (_sliderPos),
    sliderSize(_sliderSize),
    isMoving  (false),
    onScroll  (_func),
    scrollArgs(_args),
    delta     (_delta)    {}

ScrollFunc ScrollBar::getFunc() {
    return onScroll;
}
void* ScrollBar::getArgs() {
    return scrollArgs;
}

bool ScrollBar::updateSlider(MPoint shift) {
    sliderPos += shift;

    if (!moveSlider()) {
        sliderPos -= shift;
        return false;
    }

    return true;
}

bool ScrollBar::moveSlider() {
    MathRectangle scrollBar = MathRectangle(position, size);
    MathRectangle slider    = MathRectangle(sliderPos, sliderSize);

    MathRectangle intersection = getIntersection(scrollBar, slider);

    return intersection == slider;
}

bool ScrollBar::onMousePress(plugin::MouseContext context) {
    MPoint pos = MPoint(context.position);

    if (!isInside(pos)) return false;

    if (Widget::onMousePress(context)) return true;

    MathRectangle sliderRect = MathRectangle(sliderPos, sliderSize);
    if (sliderRect.isPointInside(pos)) {
        isMoving = true;
        prevPos  = pos;
        return true;
    }

    // clicked on scroll bar outside of slider
    if (delta.x != 0) {
        if (pos.x < sliderPos.x) {
            sliderPos.x -= sliderSize.x;
            if (onScroll) onScroll(scrollArgs, MPoint(-sliderSize.x * delta.x, 0));
        } else {
            sliderPos.x += sliderSize.x;
            if (onScroll) onScroll(scrollArgs, MPoint(sliderSize.x * delta.x, 0));
        }
    }
    if (delta.y != 0) {
        if (pos.y < sliderPos.y) {
            sliderPos.y -= sliderSize.y;
            if (onScroll) onScroll(scrollArgs, MPoint(0, -sliderSize.y * delta.y));
        } else {
            sliderPos.y += sliderSize.y;
            if (onScroll) onScroll(scrollArgs, MPoint(0, sliderSize.y * delta.y));
        }
    }

    return true;
}

bool ScrollBar::onMouseRelease(plugin::MouseContext context) {
    if (isMoving) {
        isMoving = false;
        return true;
    }
    return false;
}

bool ScrollBar::onMouseMove(plugin::MouseContext context) {
    MPoint pos = MPoint(context.position);

    if (isMoving) {
        if (onScroll) onScroll(scrollArgs, MPoint((pos.x - prevPos.x) * delta.x, (pos.y - prevPos.y) * delta.y));
        
        if (delta.x != 0) sliderPos.x += (pos.x - prevPos.x);
        if (delta.y != 0) sliderPos.y += (pos.y - prevPos.y);

        prevPos = pos;

        if (!moveSlider()) isMoving = false;

        return true;
    }

    return false;
}

void ScrollBar::render(RenderTarget* renderTarget) {
    // drawing area
    renderTarget->drawRect(position, size, MColor::GRAY, MColor::BLACK, regSet);
    // drawing slider
    renderTarget->drawRect(sliderPos, sliderSize, MColor::BLACK, MColor::BLACK, regSet);

    Widget::render(renderTarget);
}

void ScrollBar::move(plugin::Vec2 shift) {
    sliderPos += MPoint(shift);
    Widget::move(shift);
}

VerticalScrollBar::VerticalScrollBar(MPoint _position, MPoint _size, MPoint _sliderPos, MPoint _sliderSize, Widget* _parent, ScrollFunc _func, void* _args, MPoint _delta) :
    ScrollBar(_position, _size, _sliderPos, _sliderSize, _parent, _func, _args, _delta) {
        addButtons();
    }

void VerticalScrollBar::addButtons() {
    ScrollBtnArgs* upArgs = new ScrollBtnArgs(this, MPoint(0, -CANV_SHIFT * delta.y), MPoint(0, -CANV_SHIFT));
    ImageButton*   upBtn  = new ImageButton(MPoint(position.x + size.x - TOP_PANE_SIZE, position.y), MPoint(TOP_PANE_SIZE, SCROLLBAR_BTN_H), new MImage(UP_ARROW), this, onBtnScrollClick, upArgs, true);
    
    ScrollBtnArgs* doArgs = new ScrollBtnArgs(this, MPoint(0, CANV_SHIFT * delta.y), MPoint(0, CANV_SHIFT));
    ImageButton*   doBtn  = new ImageButton(MPoint(position.x + size.x - TOP_PANE_SIZE, position.y + size.y - SCROLLBAR_BTN_H), MPoint(TOP_PANE_SIZE, SCROLLBAR_BTN_H), new MImage(DOWN_ARROW), this, onBtnScrollClick, doArgs, true);
    
    registerObject(upBtn);
    registerObject(doBtn);
}

bool VerticalScrollBar::moveSlider() {
    MathRectangle scrollBar = MathRectangle(position + MPoint(0, SCROLLBAR_BTN_H), size - MPoint(0, 2 * SCROLLBAR_BTN_H));
    MathRectangle slider    = MathRectangle(sliderPos, sliderSize);

    MathRectangle intersection = getIntersection(scrollBar, slider);

    return intersection == slider;
}

HorizontalScrollBar::HorizontalScrollBar(MPoint _position, MPoint _size, MPoint _sliderPos, MPoint _sliderSize, Widget* _parent, ScrollFunc _func, void* _args, MPoint _delta) :
    ScrollBar(_position, _size, _sliderPos, _sliderSize, _parent, _func, _args, _delta) {
        addButtons();
    }

void HorizontalScrollBar::addButtons() {
    ScrollBtnArgs* rightArgs = new ScrollBtnArgs(this, MPoint(-CANV_SHIFT * delta.x, 0), MPoint(-CANV_SHIFT, 0));
    ImageButton*   rightBtn  = new ImageButton(MPoint(position.x, position.y + size.y - TOP_PANE_SIZE), MPoint(SCROLLBAR_BTN_W, SCROLLBAR_BTN_H), new MImage(LEFT_ARROW), this, onBtnScrollClick, rightArgs, true);

    ScrollBtnArgs* leftArgs = new ScrollBtnArgs(this, MPoint(CANV_SHIFT * delta.x, 0), MPoint(CANV_SHIFT, 0));
    ImageButton*   leftBtn  = new ImageButton(MPoint(position.x + size.x - SCROLLBAR_BTN_W, position.y + size.y - TOP_PANE_SIZE), MPoint(SCROLLBAR_BTN_W, SCROLLBAR_BTN_H), new MImage(RIGHT_ARROW), this, onBtnScrollClick, leftArgs, true);
    
    registerObject(rightBtn);
    registerObject(leftBtn);
}

bool HorizontalScrollBar::moveSlider() {
    MathRectangle scrollBar = MathRectangle(position + MPoint(SCROLLBAR_BTN_H, 0), size - MPoint(2 * SCROLLBAR_BTN_H, 0));
    MathRectangle slider    = MathRectangle(sliderPos, sliderSize);

    MathRectangle intersection = getIntersection(scrollBar, slider);

    return intersection == slider;
}

void onBtnScrollClick(void* args) {
    ScrollBtnArgs* scrollBtnArgs = (ScrollBtnArgs*) args;

    bool needScroll = scrollBtnArgs->scrollBar->updateSlider(scrollBtnArgs->sliderShift);
    if (!needScroll) return;

    void* scrollArgs = scrollBtnArgs->scrollBar->getArgs();
    scrollBtnArgs->scrollBar->getFunc()(scrollArgs, scrollBtnArgs->shift);
}