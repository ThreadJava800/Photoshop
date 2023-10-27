#ifndef _EDITBOX_h_
#define _EDITBOX_h_

#include "../renderable.h"

class EditBox : public Widget {
private:
    MFont     * font        = nullptr;
    List<char>* text        = nullptr;
    u_int       curPos      = 0;
    int       * letterWidth = nullptr;

public:
    explicit EditBox(MPoint _position, MPoint _size, Widget* _parent, MFont* _font);

    ~EditBox();

    char* getText();

    void render      (RenderTarget* renderTarget) override;
    bool onKeyPressed(MKeyboard key)              override;
};

#endif