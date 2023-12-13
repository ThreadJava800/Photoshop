#include "pluginDriver.h"

MGUI::MGUI(EventManager* _ev_man, plugin::Vec2 _size, Widget* _root) : 
    ev_manager(_ev_man), size(_size), root(_root) {
    plugin_list = new List<plugin::Plugin*>();
} 

void MGUI::addPlugin(plugin::Plugin* plugin) {
    plugin_list->pushBack(plugin);
}

plugin::WidgetI* MGUI::getRoot() const {
    return root;
}

void MGUI::createWidgetI(plugin::PluginWidgetI* widget) {
    root      ->registerSubWidget(widget->host);
    ev_manager->registerObject   (widget);
}

plugin::Plugin* MGUI::queryPlugin(uint64_t id) {
    for (size_t i = 0; i < plugin_list->getSize(); i++) {
        if ((*plugin_list)[i]->id == id) return (*plugin_list)[i];
    }

    return nullptr;
}

plugin::Texture* MGUI::loadTextureFromFile(const char *filename) {
    MImage host_texture = MImage(filename);
    return host_texture.toPluginTexture();
}

MGUI::~MGUI() {
    if (plugin_list) delete plugin_list;
}