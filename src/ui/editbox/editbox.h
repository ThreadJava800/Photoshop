#ifndef _EDITBOX_h_
#define _EDITBOX_h_

#include "../renderable.h"
#include "../../../libs/multimedia/multimedia.h"

enum INPUT_TYPE {
    NUMBERS_ONLY,
    ALL_CHARACTER,
};

typedef bool (*CheckInput)(plugin::Key);

bool chNumbersOnly(plugin::Key key);
bool chAllInput   (plugin::Key key);


class EditBox : public Widget {
private:
    MFont     * font        = nullptr;
    int         pt          = 0;
    List<char>* text        = nullptr;
    u_int       curPos      = 0;
    int       * letterWidth = nullptr;
    bool        cursorState = false;
    INPUT_TYPE  inputType   = ALL_CHARACTER;

    double getCursorX(MFont* font, int pt);

    static const CheckInput checkerFuncs[];

    inline char getRealChar(plugin::Key key);

public:
    explicit EditBox(MPoint _position, MPoint _size, Widget* _parent, MFont* _font, INPUT_TYPE _type, int _pt = BTN_TXT_PT);

    ~EditBox();

    char* getText();

    void render         (RenderTarget* renderTarget)      override;
    bool onMousePress   (plugin::MouseContext context)    override;
    bool onKeyboardPress(plugin::KeyboardContext context) override;
    bool onClock        (uint64_t delta)                  override;
};

#endif