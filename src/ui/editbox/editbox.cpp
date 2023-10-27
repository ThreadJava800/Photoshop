#include "editbox.h"

EditBox::EditBox(MPoint _position, MPoint _size, Widget* _parent, MFont* _font) :
    Widget(_position, _size, _parent),
    font  (_font),
    curPos(0)       {
        text = new List<char>();
    }

EditBox::~EditBox() {
    delete font;
    delete text;
}

void EditBox::render(RenderTarget* renderTarget) {
    ON_ERROR(!text, "Text pointer was null!",);
    ON_ERROR(!renderTarget, "Drawable area was null!",);

    char* printText = text->getCArray();

    renderTarget->drawFrame(position, size, MColor(GRAY), regSet);
    renderTarget->drawText (position, printText, MColor(GRAY), font, BTN_TXT_PT, regSet);
}

bool EditBox::onKeyPressed(MKeyboard key) {
    ON_ERROR(!text, "Text pointer was null!", false);

    size_t charCnt = text->getSize();

    if (charCnt > 0) (*text)[charCnt - 1] = key.symbol;
    else               text->pushBack(key.symbol);

    text->pushBack('\0');

    return true;
}