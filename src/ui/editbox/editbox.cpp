#include "editbox.h"

const CheckInput EditBox::checkerFuncs[] = {chNumbersOnly, chAllInput};

bool chNumbersOnly(plugin::Key key) {
    return (key >= plugin::Key::Num0 && key <= plugin::Key::Num9) || key == plugin::Key::Backspace || key == plugin::Key::Left || key == plugin::Key::Right;
}

bool chAllInput(plugin::Key key) {
    return true;
}

inline char EditBox::getRealChar(plugin::Key key) {
    if (key <= plugin::Key::Z)                                return (char)(key) + 'a';
    if (plugin::Key::Num0 <= key && key <= plugin::Key::Num9) return (char)(key) - (char)(plugin::Key::Num0) + '0';
    if (key == plugin::Key::Period) return '.';
    if (key == plugin::Key::Comma)  return ',';
    return (char)(key);
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

EditBox::EditBox(MPoint _position, MPoint _size, Widget* _parent, MFont* _font, INPUT_TYPE _type, const char* _default_text, int _pt) :
    Widget      (_position, _size, _parent),
    font        (_font),
    curPos      (0),
    cursorState (false),
    inputType   (_type),
    default_text(strdup(_default_text)),
    pt          (_pt)       {
        text = new List<char>();
    }

EditBox::~EditBox() {
    delete font;
    delete text;
    free(default_text);
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

    if (!is_active && text->getSize() == 0) renderTarget->drawText (position, default_text, MColor::GRAY, font, BTN_TXT_PT, regSet);
    else                                    renderTarget->drawText (position, printText,    MColor::GRAY, font, BTN_TXT_PT, regSet);

    int xCursorPos = getCursorX(font, BTN_TXT_PT);

    if (cursorState) renderTarget->drawLine(MPoint(xCursorPos, position.y), MPoint(xCursorPos, position.y + size.y), MColor::BLACK, regSet);
}

bool EditBox::onMousePress(plugin::MouseContext context) {
    ON_ERROR(!text, "Text pointer was null!", false);

    MPoint pos = MPoint(context.position);

    bool is_ins_bool = isInside(pos);

    if (!is_ins_bool) {
        is_active = false;
        return false;
    }

    if (!is_active && is_ins_bool) {
        is_active = true;
        return true;
    }

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

bool EditBox::onMouseRelease(plugin::MouseContext context) {
    if (!isInside(MPoint(context.position))) {
        is_active = false;
        return true;
    }
    return false;
}

bool EditBox::onKeyboardPress(plugin::KeyboardContext context) {
    ON_ERROR(!text, "Text pointer was null!", false);

    // std::cout << (int)context.key << '\n';
    if (!is_active) return false;

    if (!checkerFuncs[inputType](context.key)) return false;

    if (context.key == plugin::Key::Left) {
        curPos--;
        return true;
    }

    if (context.key == plugin::Key::Right) {
        curPos++;
        return true;
    }

    if (context.key == plugin::Key::Backspace) {
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
        text->insert  (getRealChar(context.key), curPos);
    }
    else             text->pushBack(getRealChar(context.key));

    text->pushBack('\0');
    curPos++;

    return true;
}

bool EditBox::onClock(uint64_t delta) {
    if (is_active) {
        cursorState = !cursorState;
        return true;
    }

    cursorState = false;
    return false;
}