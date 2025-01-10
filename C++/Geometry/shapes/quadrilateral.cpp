#include "quadrilateral.h"

void Quadrilateral::quadDotChecks() const
{
    std::set<Dot> checkSameDots((*pDots).begin(), (*pDots).end());
    *pDots = std::vector<Dot>(checkSameDots.begin(), checkSameDots.end());
    if ((*pDots).size() != 4)
    {
        throw invalidDots("Not 4 dots in a quadrilateral constructor\n");
    }
    int leftBottomDotIndex = 0;
    for (int i = 1; i < 4; ++i)
    {
        if ((*pDots)[i].x <= (*pDots)[leftBottomDotIndex].x && 
            (*pDots)[i].y <= (*pDots)[leftBottomDotIndex].y )
        {
            leftBottomDotIndex = i;
        }
    }
    std::swap((*pDots)[0], (*pDots)[leftBottomDotIndex]);
    Vector oX(1, 0);
    std::sort((*pDots).begin() + 1, (*pDots).end(), [this, oX](auto& a, auto& b)
    {
        double angle1 = getAngle(oX, Vector((*pDots)[0], a));
        double angle2 = getAngle(oX, Vector((*pDots)[0], b));
        return angle1 == angle2 ? 
            getDistance((*pDots)[0], a) < getDistance((*pDots)[0], b) :
            angle1 < angle2;
    });
}

Quadrilateral::Quadrilateral(const std::vector<Dot>& dots, bool)
        : ConvexPolygon(dots, true)
{}

Quadrilateral::Quadrilateral(const std::vector<Dot>& dots)
        : ConvexPolygon(dots, true)
{
    quadDotChecks();
}

Quadrilateral::Quadrilateral(
    const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4)
        : ConvexPolygon({d1, d2, d3, d4}, true)
{
    quadDotChecks();
}

double Quadrilateral::area() const 
{
    return ConvexPolygon::area();
}

double Quadrilateral::perimeter() const 
{
    return ConvexPolygon::perimeter();
}

bool Quadrilateral::containsDot(const Dot& dot) const 
{
    return ConvexPolygon::containsDot(dot);
}

Quadrilateral::~Quadrilateral() {}