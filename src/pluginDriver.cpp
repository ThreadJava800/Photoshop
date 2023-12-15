#include "pluginDriver.h"

PluginWidget::PluginWidget(Widget* _parent, plugin::PluginWidgetI* _plugin_widget) :
    Widget(MPoint(), MPoint(), _parent, _plugin_widget->getPriority()), 
    plugin_widget(_plugin_widget) {}

PluginWidget::~PluginWidget() {
    delete plugin_widget;
}

// void PluginWidget::registerSubWidget(plugin::WidgetI* object) {

// }

// void PluginWidget::unregisterSubWidget(plugin::WidgetI* object) {

// }

// plugin::Vec2 PluginWidget::getSize() const {

// }

// void PluginWidget::setSize(plugin::Vec2) {

// }

// plugin::Vec2 PluginWidget::getPos() const {

// }

// void PluginWidget::setPos(plugin::Vec2) {

// }

// void PluginWidget::setParent(WidgetI *root) {

// }

// plugin::WidgetI* PluginWidget::getParent() const {

// }

// void PluginWidget::move(plugin::Vec2 shift) {

// }

// bool PluginWidget::getAvailable() const {

// }

// void PluginWidget::setAvailable(bool _new_available) {
//     plugin_widget->se
// }


bool PluginWidget::onKeyboardPress(plugin::KeyboardContext context) {
    bool res = Widget::onKeyboardPress(context);
    if (res) return res;

    return plugin_widget->onKeyboardPress(context);
}

bool PluginWidget::onKeyboardRelease(plugin::KeyboardContext context) {
    bool res = Widget::onKeyboardRelease(context);
    if (res) return res;

    return plugin_widget->onKeyboardRelease(context);
}

bool PluginWidget::onClock(uint64_t delta) {
    bool res = Widget::onClock(delta);
    if (res) return res;

    return plugin_widget->onClock(delta);
}

bool PluginWidget::onMousePress(plugin::MouseContext context) {
    bool res = Widget::onMousePress(context);
    if (res) return res;

    return plugin_widget->onMousePress(context);
}

bool PluginWidget::onMouseRelease(plugin::MouseContext context) {
    bool res = Widget::onMouseRelease(context);
    if (res) return res;

    return plugin_widget->onMouseRelease(context);
}

bool PluginWidget::onMouseMove(plugin::MouseContext context) {
    bool res = Widget::onMouseMove(context);
    if (res) return res;

    return plugin_widget->onMouseMove(context);
}

void PluginWidget::render(RenderTarget* rt) {
    plugin_widget->render(rt);
    Widget::render(rt);
}

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
    PluginWidget* plugin_widget = new PluginWidget(root, widget);
    widget->host = plugin_widget;
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