#ifndef _SUBMENU_h_
#define _SUBMENU_h_

#include "../renderable.h"

class SubMenu : public Widget {
public:
    explicit SubMenu(MPoint _position, MPoint _size, Widget* _parent);

    ~SubMenu();

    void changeActivity();

    void       render          (RenderTarget* renderTarget) override;
    RegionSet* getDefaultRegSet()                           override;
};

#endif