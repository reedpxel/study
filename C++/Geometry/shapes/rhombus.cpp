#include "rhombus.h"

void Rhombus::rhombusCheck(const Segment& s1, const Segment& s2) const
{
    if (s1.length() != s2.length())
    {
        throw invalidDots("Adjacent sides are not equal\n");
    }
}

Rhombus::Rhombus(const std::vector<Dot>& dots, bool)
        : Parallelogramm(dots, true)
{}

Rhombus::Rhombus(const std::vector<Dot>& dots)
        : Parallelogramm(dots)
{
    rhombusCheck(Segment((*pDots)[0], (*pDots)[1]), 
        Segment((*pDots)[1], (*pDots)[2]));
}

Rhombus::Rhombus(const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4)
        : Parallelogramm(std::vector<Dot>({d1, d2, d3, d4}))
{
    rhombusCheck(Segment((*pDots)[0], (*pDots)[1]), 
        Segment((*pDots)[1], (*pDots)[2]));
}

double Rhombus::area() const 
{
    Segment s1((*pDots)[0], (*pDots)[1]);
    Segment s2((*pDots)[1], (*pDots)[2]);
    return s1.length() * s2.length() * sin(getAngle(Vector(s1), Vector(s2)));
}

double Rhombus::perimeter() const
{
    return 4 * Segment((*pDots)[0], (*pDots)[1]).length();
}

bool Rhombus::containsDot(const Dot& dot) const
{
    return ConvexPolygon::containsDot(dot);
}

Rhombus::~Rhombus() {}