#ifndef _WINDOW_h_
#define _WINDOW_h_

#include "../../includes.h"
#include "../renderable.h"
#include "../menu/menu.h"

class Window : public Widget {
private:
    Menu*  actions  = nullptr;
    
    void createTopPanel();
    void createTestWindow();
    
public:
    // temp fields
    MColor col = MColor(DEFAULT_BACK_COL);
    bool isVisible = true;

    explicit Window(MPoint _position, MPoint _size);
    explicit Window(MPoint _position, MPoint _size, Menu* _actions);
    ~Window();

    void render(RenderTarget* renderTarget, RegionSet* regions) override;
};

void onMove(Window* window, MPoint newPos, MPoint oldPos);
void closeFunc(void* window);

#endif