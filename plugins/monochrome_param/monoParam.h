#ifndef _MONO_PARAM_FILTER_h_
#define _MONO_PARAM_FILTER_h_

#include <iostream>
#include "../plugin.h"

class MonochromeParamFilter : public plugin::FilterI {
private:
    plugin::Array<const char *> param_names;
    double r = 0.3, g = 0.6, b = 0.1;

public:
    explicit MonochromeParamFilter();

    void                        apply        (plugin::RenderTargetI *data)  override;
    plugin::Array<const char *> getParamNames()                             override;
    plugin::Array<double>       getParams    ()                             override;
    void                        setParams    (plugin::Array<double> params) override; 
};

class MonochromeParamPlugin : public plugin::Plugin {
private:
    plugin::App*      app;
    MonochromeParamFilter* filter;

public:
    explicit MonochromeParamPlugin();

    plugin::Interface *getInterface() override;
};

#endif