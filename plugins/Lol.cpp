#include "Lol.h"

extern "C" plugin::Plugin* getInstance(plugin::App *app)
{
    return new WhiteAndBlackPlugin(app);
}

WhiteAndBlackPlugin::WhiteAndBlackPlugin(plugin::App* app) :
app (app)
{ 
    filter = new WhiteAndBlackFilter();

    name = "Lol";
    type = plugin::InterfaceType::Filter;
}

plugin::Array<const char*> WhiteAndBlackFilter::getParamNames()
{ 
    return {0, nullptr}; 
}

plugin::Array<double> WhiteAndBlackFilter::getParams()
{ 
    return {0, nullptr}; 
}

void WhiteAndBlackFilter::setParams(plugin::Array<double> params)
{ }

void fuck()
{}

void WhiteAndBlackFilter::apply(plugin::RenderTargetI* rt)
{
    plugin::Texture* texture = rt->getTexture(); 
    plugin::Color*   pixels  = texture->pixels;

    fprintf(stderr, "inside plugin size = (%d, %d)\n", texture->width, texture->height);


    for (int y = 0; y < texture->height; y++)
    {
        for (int x = 0; x < texture->width; x++)
        {
            plugin::Color* cur_color = &pixels[y * texture->width + x];

            int pixel_color = cur_color->r * 0.3 + 
                              cur_color->g * 0.6 + 
                              cur_color->b * 0.1;
            cur_color->r = pixel_color;
            cur_color->g = pixel_color;
            cur_color->b = pixel_color;
        }
    }


    plugin::Vec2 pos  = {100, 100};
    plugin::Vec2 size = {texture->width, texture->height};
    fprintf(stderr, "pos  = (%lg, %lg)\n", pos.x, pos.y);
    fprintf(stderr, "size = (%lg, %lg)\n", size.x, size.y);
    fprintf(stderr, "text = (%p, %lg, %lg)\n", texture->pixels, texture->height, texture->width);

    fuck();

    rt->drawTexture(24, pos, size, texture, 42);
    delete[] texture->pixels;
    delete texture;
}
