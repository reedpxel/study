#include "square.h"

Square::Square(const std::vector<Dot>& dots) 
        : Parallelogramm(dots) // checks up to parallelogramm done here
        , Rectangle(dots) // a check for rectangle
        , Rhombus(dots) // a check for rhombus
{}

Square::Square(const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4) 
        : Parallelogramm(std::vector<Dot>({d1, d2, d3, d4}))
        , Rectangle(std::vector<Dot>({d1, d2, d3, d4})) 
        , Rhombus(std::vector<Dot>({d1, d2, d3, d4}))
{}

double Square::area() const
{
    double side = Segment((*pDots)[0], (*pDots)[1]).length(); 
    return side * side;
}

double Square::perimeter() const
{
    double side = Segment((*pDots)[0], (*pDots)[1]).length(); 
    return 4 * side;
}

bool Square::containsDot(const Dot& dot) const
{
    return ConvexPolygon::containsDot(dot);
}

Square::~Square() {}