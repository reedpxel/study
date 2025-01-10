#include "rectangle.h"

void Rectangle::rectangleCheck(const Segment& s1, const Segment& s2) const
{
    if (getAngle(Vector(s1), Vector(s2)) != 90)
    {
        throw invalidDots("Sides are not perpendicular\n");
    }
}

Rectangle::Rectangle(const std::vector<Dot>& dots, bool)
        : Parallelogramm(dots) // checks are done
{}

Rectangle::Rectangle(const std::vector<Dot>& dots)
        : Parallelogramm(dots) // here done checks for quad and parallelogramm
{
    rectangleCheck(Segment((*pDots)[0], (*pDots)[1]), 
        Segment((*pDots)[1], (*pDots)[2]));
}

Rectangle::Rectangle(const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4)
        : Parallelogramm(std::vector<Dot>({d1, d2, d3, d4}))
{
    rectangleCheck(Segment((*pDots)[0], (*pDots)[1]), 
        Segment((*pDots)[1], (*pDots)[2]));
}

double Rectangle::area() const
{
    return Segment((*pDots)[0], (*pDots)[1]).length() *  
        Segment((*pDots)[1], (*pDots)[2]).length();
}

double Rectangle::perimeter() const
{
    return Parallelogramm::perimeter();
}

bool Rectangle::containsDot(const Dot& dot) const
{
    return ConvexPolygon::containsDot(dot);
}

Rectangle::~Rectangle() {}