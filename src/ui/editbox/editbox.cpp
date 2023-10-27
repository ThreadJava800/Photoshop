#include "editbox.h"

double EditBox::getCursorX(MFont* font, int pt) {
    size_t charCnt = text->getSize();
    if (charCnt == 0) return position.x;

    double shiftConst = getTextSize(text->getCArray(), font, pt).x / charCnt;
    double posX = position.x + shiftConst * (charCnt);

    // for (size_t i = 0; i < charCnt - 1; i++) {
    //     posX += (getTextSize(text->getCArray(), font, pt).x / charCnt);
    // }

    return posX;
}

EditBox::EditBox(MPoint _position, MPoint _size, Widget* _parent, MFont* _font) :
    Widget     (_position, _size, _parent),
    font       (_font),
    curPos     (0),
    cursorState(false)       {
        text = new List<char>();
    }

EditBox::~EditBox() {
    delete font;
    delete text;
}

char* EditBox::getText() {
    ON_ERROR(!text, "Pointer to str was null!", nullptr);

    return text->getCArray();
}

void EditBox::render(RenderTarget* renderTarget) {
    ON_ERROR(!text, "Text pointer was null!",);
    ON_ERROR(!renderTarget, "Drawable area was null!",);

    char* printText = text->getCArray();

    renderTarget->drawFrame(position, size, MColor(GRAY), regSet);
    renderTarget->drawText (position, printText, MColor(GRAY), font, BTN_TXT_PT, regSet);

    int xCursorPos = getCursorX(new MFont(DEFAULT_FONT), BTN_TXT_PT);

    if (cursorState) renderTarget->drawLine(MPoint(xCursorPos, position.y), MPoint(xCursorPos, position.y + size.y), MColor(BLACK), regSet);
}

bool EditBox::onKeyPressed(MKeyboard key) {
    ON_ERROR(!text, "Text pointer was null!", false);

    if (!(key.symbol >= '0' && key.symbol <= '9' || key.symbol == BACKSPACE)) return false;

    if (key.symbol == BACKSPACE) {
        size_t charCnt = text->getSize();
        if (charCnt >= 2) {
            text->remove(charCnt - 2);
        }

        return true;
    }

    size_t charCnt = text->getSize();

    if (charCnt > 0) (*text)[charCnt - 1] = key.symbol;
    else               text->pushBack(key.symbol);

    text->pushBack('\0');

    return true;
}

bool EditBox::onTimerTick(double delta) {
    cursorState = !cursorState;

    return true;
}