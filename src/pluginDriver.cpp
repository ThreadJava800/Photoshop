#include "pluginDriver.h"

MGUI::MGUI(EventManager* _ev_man, plugin::Vec2 _size, Widget* _root) : 
    ev_manager(_ev_man), size(_size), root(_root) {} 

plugin::WidgetI* MGUI::getRoot() const {
    return root;
}

void MGUI::createWidgetI(plugin::PluginWidgetI* widget) {

}

plugin::Plugin* MGUI::queryPlugin(uint64_t id) {

}

plugin::Texture* MGUI::loadTextureFromFile(const char *filename) {

}