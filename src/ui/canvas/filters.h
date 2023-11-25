#ifndef _FILTERS_h_
#define _FILTERS_h_

#include "../../../libs/multimedia/multimedia.h"
#include "../../plugin.h"

class BrightnessFilter : public plugin::FilterI {
private:
    int                         changeValue = 0;
    plugin::Array<const char *> param_names;

public:
    explicit BrightnessFilter();

    void                        apply        (plugin::RenderTargetI *data)  override;
    plugin::Array<const char *> getParamNames()                             override;
    plugin::Array<double>       getParams    ()                             override;
    void                        setParams    (plugin::Array<double> params) override;

    ~BrightnessFilter() {}
};

// class MonochromeFilter : public plugin::FilterI {
// private:
//     plugin::Array<const char *> param_names;

// public:
//     explicit MonochromeFilter();

//     void                        apply        (plugin::RenderTargetI *data)  override;
//     plugin::Array<const char *> getParamNames()                             override;
//     plugin::Array<double>       getParams    ()                             override;
//     void                        setParams    (plugin::Array<double> params) override;  
// };

class ColorfulnessFilter : public plugin::FilterI {
private:
    plugin::Array<const char *> param_names;

    double saturCoeff  = 1.0;

    static constexpr double RCOEFF = 0.299;
    static constexpr double GCOEFF = 0.587;
    static constexpr double BCOEFF = 0.114;

public:
    static constexpr double SATUR_UP   = 2.0;
    static constexpr double SATUR_DOWN = 0.5;

    explicit ColorfulnessFilter();

    void                        apply        (plugin::RenderTargetI *data)  override;
    plugin::Array<const char *> getParamNames()                             override;
    plugin::Array<double>       getParams    ()                             override;
    void                        setParams    (plugin::Array<double> params) override;

    ~ColorfulnessFilter() {}
};

class FilterManager : public plugin::FilterManagerI {
private:
    bool                   need_free  = false;
    bool                   active     = false;
    plugin::FilterI*       lastFilter = nullptr;
    plugin::RenderTargetI* rt         = nullptr;

public:
    explicit FilterManager();

    ~FilterManager();

    void                   setActive  (bool _active);
    void                   setNeedFree(bool _need_free);
    plugin::RenderTargetI* getRT      ();
    plugin::FilterI*       getLast    ();
    bool                   getActive  ();

    void setRenderTarget(plugin::RenderTargetI *target) override;
    void setFilter      (plugin::FilterI *filter)       override;
    void applyFilter    ()                              override;
};

#endif