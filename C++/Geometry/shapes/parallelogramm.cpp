#include "parallelogramm.h"

void Parallelogramm::parallelogrammCheck(const Segment& s1, const Segment& s2, 
    const Segment& s3, const Segment& s4) const
{
    if (intersect(Line(s1), Line(s3)) || intersect(Line(s2), Line(s4)))
    {
        throw invalidDots("Opposite sides are not parallel\n");
    }
}

Parallelogramm::Parallelogramm(const std::vector<Dot>& dots, bool)
        : Trapezium(dots, true)
{}

Parallelogramm::Parallelogramm(const std::vector<Dot>& dots)
        : Trapezium(dots, true)
{
    quadDotChecks();
    parallelogrammCheck(Segment((*pDots)[0], (*pDots)[1]), 
        Segment((*pDots)[1], (*pDots)[2]), 
        Segment((*pDots)[2], (*pDots)[3]), 
        Segment((*pDots)[3], (*pDots)[0]));
}

Parallelogramm::Parallelogramm(const Dot& d1, const Dot& d2, const Dot& d3, 
    const Dot& d4)
        : Trapezium(std::vector<Dot>({d1, d2, d3, d4}), true)
{
    quadDotChecks();
    parallelogrammCheck(Segment((*pDots)[0], (*pDots)[1]), 
        Segment((*pDots)[1], (*pDots)[2]), 
        Segment((*pDots)[2], (*pDots)[3]), 
        Segment((*pDots)[3], (*pDots)[0]));
}

double Parallelogramm::area() const
{
    Segment base((*pDots)[0], (*pDots)[1]);
    Segment oppositeToBase((*pDots)[2], (*pDots)[3]);
    return base.length() * getDistance(base, oppositeToBase);
}

double Parallelogramm::perimeter() const
{
    return 2 * (Segment((*pDots)[0], (*pDots)[1]).length() + 
        Segment((*pDots)[1], (*pDots)[2]).length());
}

bool Parallelogramm::containsDot(const Dot& dot) const
{
    return ConvexPolygon::containsDot(dot);
}

Parallelogramm::~Parallelogramm() {}