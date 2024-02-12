#ifndef _FILTERS_h_
#define _FILTERS_h_

#include "../../../libs/multimedia/multimedia.h"
#include "../../plugin.h"

static const char* BRIGHTNESS_PARAM_NAMES  [] = {"Brightness"};
static const char* COLORFULNESS_PARAM_NAMES[] = {"Colorfulness"};

class BrightnessFilter : public plugin::FilterI {
private:
    bool                        need_argument;
    int                         changeValue = BRIGHTNESS_SHIFT;
    plugin::Array<const char *> param_names;

public:
    explicit BrightnessFilter(bool _need_argument);

    void                        apply        (plugin::RenderTargetI *data)  override;
    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;
};

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

    explicit ColorfulnessFilter(double _satur_coeff);

    void                        apply        (plugin::RenderTargetI *data)  override;
    plugin::Array<const char *> getParamNames() const                       override;
    plugin::Array<double>       getParams    () const                       override;
    void                        setParams    (plugin::Array<double> params) override;
};

class FilterManager {
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

    void setRenderTarget(plugin::RenderTargetI *target);
    void setFilter      (plugin::FilterI *filter);
    void applyFilter    ();
};

#endif