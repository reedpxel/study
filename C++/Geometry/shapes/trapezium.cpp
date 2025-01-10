#include "trapezium.h"

void Trapezium::trapeziumCheck() const
{
    Line side1((*pDots)[0], (*pDots)[1]);
    Line side2((*pDots)[1], (*pDots)[2]);
    Line side3((*pDots)[2], (*pDots)[3]);
    Line side4((*pDots)[3], (*pDots)[0]);
    if (!intersect(side1, side3))
    {
    } else if (!intersect(side2, side4)) {
        // Parallel sides must be side1 and side 3. 
        // If it is not the case, dots are shifted by 1
        for (int i = 0; i < 3; ++i)
        {
            std::swap((*pDots)[i], (*pDots)[i + 1]);
        }
    } else throw ConvexPolygon::invalidDots("No parallel sides in trapezium\n");
}

Trapezium::Trapezium(const std::vector<Dot>& dots, bool)
        : Quadrilateral(dots, true)
{}

Trapezium::Trapezium(const std::vector<Dot>& dots)
        : Quadrilateral(dots)
{
    trapeziumCheck();
}

Trapezium::Trapezium(
    const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4)
        : Quadrilateral(std::vector<Dot>({d1, d2, d3, d4}))
{
    trapeziumCheck();
}

double Trapezium::area() const
{
    Segment base1((*pDots)[0], (*pDots)[1]);
    Segment base2((*pDots)[2], (*pDots)[3]);
    return (base1.length() + base2.length()) / 2 * getDistance(base1, base2);
}

double Trapezium::perimeter() const
{
    return getDistance((*pDots)[0], (*pDots)[1]) + 
        getDistance((*pDots)[1], (*pDots)[2]) +
        getDistance((*pDots)[2], (*pDots)[3]) +
        getDistance((*pDots)[3], (*pDots)[0]);
}

bool Trapezium::containsDot(const Dot& dot) const
{
    return ConvexPolygon::containsDot(dot);
}

Trapezium::~Trapezium() {}