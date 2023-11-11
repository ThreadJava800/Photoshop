#include "editbox.h"

const CheckInput EditBox::checkerFuncs[] = {chNumbersOnly, chAllInput};

bool chNumbersOnly(MKeyboard key) {
    return key.symbol >= '0' && key.symbol <= '9' || key.symbol == BACKSPACE || key.keyId == LEFT_KEY || key.keyId == RIGHT_KEY;
}

bool chAllInput(MKeyboard key) {
    return true;
}

double EditBox::getCursorX(MFont* font, int pt) {
    size_t charCnt = text->getSize();
    if (charCnt == 0) return position.x;

    double shiftConst = getTextSize(text->getCArray(), font, pt).x / (charCnt - 1);
    double posX = position.x + shiftConst * curPos;

    // for (size_t i = 0; i < charCnt - 1; i++) {
    //     posX += (getTextSize(text->getCArray(), font, pt).x / charCnt);
    // }

    return posX;
}

EditBox::EditBox(MPoint _position, MPoint _size, Widget* _parent, MFont* _font, INPUT_TYPE _type, int _pt) :
    Widget     (_position, _size, _parent),
    font       (_font),
    curPos     (0),
    cursorState(false),
    inputType  (_type),
    pt         (_pt)       {
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

    renderTarget->drawFrame(position, size, MColor::GRAY, regSet);
    renderTarget->drawText (position, printText, MColor::GRAY, font, BTN_TXT_PT, regSet);

    int xCursorPos = getCursorX(font, BTN_TXT_PT);

    if (cursorState) renderTarget->drawLine(MPoint(xCursorPos, position.y), MPoint(xCursorPos, position.y + size.y), MColor::BLACK, regSet);
}

bool EditBox::onMousePressed(MPoint pos, MMouse btn) {
    ON_ERROR(!text, "Text pointer was null!", false);

    size_t charCnt    = text->getSize();
    double shiftConst = getTextSize(text->getCArray(), font, pt).x / (charCnt - 1);

    for (size_t i = 0; i < charCnt - 1; i++) {
        if (i * shiftConst <= (pos - position).x && (pos - position).x <= (i + 1) * shiftConst) {
            curPos = i + 1;
            break;
        }
    }

    if ((pos - position).x > charCnt * shiftConst) curPos = charCnt - 1;

    return true;
}

bool EditBox::onKeyPressed(MKeyboard key) {
    ON_ERROR(!text, "Text pointer was null!", false);

    if (!checkerFuncs[inputType](key)) return false;

    if (key.keyId == LEFT_KEY) {
        curPos--;
        return true;
    }

    if (key.keyId == RIGHT_KEY) {
        curPos++;
        return true;
    }

    if (key.symbol == BACKSPACE) {
        size_t charCnt = text->getSize();
        if (charCnt >= 2 && curPos > 0) {
            text->remove(curPos - 1);
        }

        curPos--;

        return true;
    }

    size_t charCnt = text->getSize();

    // values are out from frame
    double shiftConst = getTextSize(text->getCArray(), font, pt).x / (charCnt - 1);
    if (charCnt * shiftConst > size.x) return true;

    if (charCnt > 0) {
        text->pop();
        text->insert  (key.symbol, curPos);
    }
    else             text->pushBack(key.symbol);

    text->pushBack('\0');
    curPos++;

    return true;
}

bool EditBox::onTimerTick(double delta) {
    cursorState = !cursorState;

    return true;
}