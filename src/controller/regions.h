#ifndef _REGIONS_h_
#define _REGIONS_h_

#include "../../libs/multimedia/multimedia.h"

class RegionSet;
class MathRectangle {
private:
    MPoint position = MPoint();
    MPoint size     = MPoint();

public:
    explicit MathRectangle(MPoint _pos, MPoint _size);
    ~MathRectangle();

    double top();
    double l();
    double r();
    double down(); 

    MPoint getPosition();
    MPoint getSize    ();

    bool isYInside(double yPoint);
    bool isXInside(double xPoint);

    friend bool          isIntersected  (MathRectangle posOld, MathRectangle posNew);
    friend MathRectangle getIntersection(MathRectangle posOld, MathRectangle posNew);
    friend RegionSet*    merge          (MathRectangle posOld, MathRectangle posNew);
    friend RegionSet*    diff           (MathRectangle posOld, MathRectangle posNew);
};

class RegionSet {
private:
   List<MathRectangle>* rectangles = nullptr;

public:
    explicit RegionSet();
    ~RegionSet();

    void addRegion(MathRectangle region);

    size_t getSize();
    List<MathRectangle>* getRectangles();
    void visualize       (RenderTarget* renderTarget);
};

#endif