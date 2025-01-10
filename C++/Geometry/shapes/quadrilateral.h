#ifndef QUADRILATERAL_H
#define QUADRILATERAL_H
#include "convex_polygon.h"

class Quadrilateral : public ConvexPolygon
{
protected:
    void quadDotChecks() const;
    Quadrilateral(const std::vector<Dot>& dots, bool);
public:
    Quadrilateral(const std::vector<Dot>& dots);
    Quadrilateral(const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4);
    double area() const override;
    double perimeter() const override;
    bool containsDot(const Dot& dot) const override;
    ~Quadrilateral() override;
};

#endif
