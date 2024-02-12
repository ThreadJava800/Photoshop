#ifndef _MONO_FILTER_h_
#define _MONO_FILTER_h_

#include <iostream>
#include "../plugin.h"

class MonochromeFilter : public plugin::FilterI {
private:
    plugin::Array<const char *> param_names;

public:
    explicit MonochromeFilter();

    void                        apply        (plugin::RenderTargetI *data)        override;
    plugin::Array<const char *> getParamNames()                             const override;
    plugin::Array<double>       getParams    ()                             const override;
    void                        setParams    (plugin::Array<double> params)       override; 
};

class MonochromePlugin : public plugin::Plugin {
private:
    plugin::App*      app;
    MonochromeFilter* filter;

public:
    explicit MonochromePlugin();

    plugin::Interface *getInterface() const override;
    void               selectPlugin()       override;
};

#endif