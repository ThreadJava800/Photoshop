#include "pluginDriver.h"

MGUI::MGUI(plugin::Vec2 _size, plugin::WidgetI* _root) : size(_size), root(_root) {} 

plugin::Vec2 MGUI::getSize() {
    return size;
}

plugin::RenderTargetI* MGUI::getRenderTarget(plugin::Vec2 size, plugin::Vec2 pos, plugin::Plugin *self) {
    RenderTarget* rend_target = new RenderTarget(MPoint(pos), MPoint(size));
    return rend_target;
}

void MGUI::createParamWindow(plugin::Array<const char *> param_names, plugin::Interface * self) {
    
}

plugin::WidgetI* MGUI::getRoot() {
    return root;
}