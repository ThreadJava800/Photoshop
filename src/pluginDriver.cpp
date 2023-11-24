#include "pluginDriver.h"

MGUI::MGUI(EventManager* _ev_man, plugin::Vec2 _size, Widget* _root) : 
    ev_manager(_ev_man), size(_size), root(_root) {} 

plugin::Vec2 MGUI::getSize() {
    return size;
}

plugin::RenderTargetI* MGUI::getRenderTarget(plugin::Vec2 size, plugin::Vec2 pos, plugin::Plugin *self) {
    return new RenderTarget(MPoint(pos), MPoint(size));
}

void MGUI::createParamWindow(plugin::Array<const char *> param_names, plugin::Interface * self) {
    PluginParamWindow* param_win = new PluginParamWindow(ev_manager, root, param_names, self);
    ((Widget*)root)->registerSubWidget(param_win);
}

plugin::WidgetI* MGUI::getRoot() {
    return root;
}