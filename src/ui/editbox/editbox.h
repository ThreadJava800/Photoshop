#ifndef _EDITBOX_h_
#define _EDITBOX_h_

#include "../renderable.h"
#include "../../../libs/multimedia/multimedia.h"

enum INPUT_TYPE {
    NUMBERS_ONLY,
    ALL
};

typedef bool (*CheckInput)(MKeyboard);

bool chNumbersOnly(MKeyboard key);
bool chAllInput   (MKeyboard key);


class EditBox : public Widget {
private:
    MFont     * font        = nullptr;
    int         pt          = 0;
    List<char>* text        = nullptr;
    u_int       curPos      = 0;
    int       * letterWidth = nullptr;
    bool        cursorState = false;
    INPUT_TYPE  inputType   = ALL;

    double getCursorX(MFont* font, int pt);

    static const CheckInput checkerFuncs[];

public:
    explicit EditBox(MPoint _position, MPoint _size, Widget* _parent, MFont* _font, INPUT_TYPE _type, int _pt = BTN_TXT_PT);

    ~EditBox();

    char* getText();

    void render        (RenderTarget* renderTarget) override;
    bool onMousePressed(MPoint pos, MMouse btn)     override;
    bool onKeyPressed  (MKeyboard key)              override;
    bool onTimerTick   (double delta)               override;
};

#endif