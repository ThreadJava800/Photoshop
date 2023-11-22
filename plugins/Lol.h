#include "plugin.h"

#include <cstdio>

extern "C" plugin::Plugin* getInstance(plugin::App *app);

class WhiteAndBlackFilter;

class WhiteAndBlackPlugin : public plugin::Plugin
{
    plugin::App*         app;
    WhiteAndBlackFilter* filter;


public:
    WhiteAndBlackPlugin(plugin::App* app);

    plugin::Interface *getInterface() override
    { return (plugin::Interface*) filter; }
    
    ~WhiteAndBlackPlugin()
    {}
};

class WhiteAndBlackFilter : public plugin::FilterI
{
    plugin::Array<const char*> param_names;

public:
    void                       apply        (plugin::RenderTargetI* rt)    override;
    void                       setParams    (plugin::Array<double> params) override;
    plugin::Array<const char*> getParamNames()                             override;
    plugin::Array<double>      getParams    ()                             override;

    ~WhiteAndBlackFilter()
    {}
};

