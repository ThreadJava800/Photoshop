#ifndef _FILTERS_h_
#define _FILTERS_h_

#include "../../../libs/multimedia/multimedia.h"

class Filter {
public:
    virtual ~Filter() = default;

    virtual void               apply        (RenderTarget* rt)     = 0;
    virtual List<double>*      getParams    ()                     = 0;
    virtual void               setParams    (List<double>& params) = 0;
    virtual List<const char*>* getParamNames()                     = 0;
};

class BrightnessFilter : public Filter {
private:
    int                changeValue = 0;
    List<const char*>* paramNames  = nullptr;

public:
    explicit BrightnessFilter();

    ~BrightnessFilter();

    void               apply        (RenderTarget* rt)     override;
    List<double>*      getParams    ()                     override;
    void               setParams    (List<double>& params) override;
    List<const char*>* getParamNames()                     override;
};

class MonochromeFilter : public Filter {
private:
    List<const char*>* paramNames  = nullptr;

public:
    explicit MonochromeFilter();

    ~MonochromeFilter();

    void               apply        (RenderTarget* rt)     override;
    List<double>*      getParams    ()                     override;
    void               setParams    (List<double>& params) override;
    List<const char*>* getParamNames()                     override;    
};

class ColorfulnessFilter : public Filter {
private:
    double             saturCoeff  = 1.0;
    List<const char*>* paramNames  = nullptr;

    static constexpr double RCOEFF = 0.299;
    static constexpr double GCOEFF = 0.587;
    static constexpr double BCOEFF = 0.114;

public:
    static constexpr double SATUR_UP   = 2.0;
    static constexpr double SATUR_DOWN = 0.5;

    explicit ColorfulnessFilter();

    ~ColorfulnessFilter();

    void               apply        (RenderTarget* rt)     override;
    List<double>*      getParams    ()                     override;
    void               setParams    (List<double>& params) override;
    List<const char*>* getParamNames()                     override;
};

class FilterManager {
private:
    bool          active     = false;
    Filter*       lastFilter = nullptr;
    RenderTarget* rt         = nullptr;

public:
    explicit FilterManager();

    ~FilterManager();

    void setRT    (RenderTarget* _rt);
    void setLast  (Filter* _filter);
    void setActive(bool _active);

    RenderTarget* getRT    ();
    Filter*       getLast  ();
    bool          getActive();

    void applyFilter();
};

#endif