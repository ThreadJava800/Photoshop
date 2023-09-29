#include "renderable.h"

Widget::Widget(MPoint _position) :
    position(_position) {
        subWindows = new List<Widget*>();
    }

Widget::Widget(MPoint _position, List<Widget*>* _subWindows) :
    position  (_position),
    subWindows(_subWindows)  {}

Widget::~Widget() {
    delete subWindows;

    subWindows = nullptr;
}