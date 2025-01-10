#include "dot.h"

Dot::Dot(double x, double y) : x(x), y(y) {}

bool Dot::operator<(const Dot& other) const
{
    return x == other.x ? y < other.y : x < other.x;
}

bool Dot::operator==(const Dot& other) const
{ 
    return this->x == other.x && this->y == other.y; 
}

bool Dot::operator!=(const Dot& other) const
{ 
    return !(*this == other); 
}

bool Dot::liesOnLine(const Line& l) const 
{ 
    return !(l.a * x + l.b * y + l.c); 
}

bool Dot::liesOnSegment(const Segment& s) const
{
    if (s.x1 == s.x2 && s.y1 == s.y2) 
    {
        return this->x == s.x1 && this->y == s.y1;
    }
    Vector ab(Dot(s.x1, s.y1), Dot(s.x2, s.y2));
    Vector ba = -ab;
    Vector ac(Dot(s.x1, s.y1), Dot(x, y));
    Vector bc(Dot(s.x2, s.y2), Dot(x, y));
    return areCodirectional(ab, ac) && areCodirectional(ba, bc);
}

double getDistance(const Dot& d1, const Dot& d2) 
{ 
    return sqrt(pow(d1.x - d2.x, 2) + pow(d1.y - d2.y, 2)); 
}

double getDistance(const Line& l1, const Line& l2)
{
    if (intersect(l1, l2)) return 0;
    double x1, y1;
    if (!l1.b)
    {
        x1 = -l1.c;
        y1 = 0;
    } else {
        x1 = 0;
        y1 = -l1.c / l1.b;
    }
    return getDistance(Dot(x1, y1), l2);
}

double getDistance(const Segment& s1, const Segment& s2)
{
    if (intersect(s1, s2)) return 0;
    Dot a(s1.x1, s1.y1);
    Dot b(s1.x2, s1.y2);
    Dot c(s2.x1, s2.y1);
    Dot d(s2.x2, s2.y2);
    return std::min
    (
        std::min(getDistance(a, s2), getDistance(b, s2)), 
        std::min(getDistance(c, s1), getDistance(d, s1))
    );
}

double getDistance(const Dot& d, const Line& l)
{
    if (!(l.a * d.x + l.b * d.y + l.c)) return 0;
    double x1, y1, x2, y2;
    if (!l.b){
        y1 = 0;
        y2 = 1;
        x1 = x2 = -l.c;
    } else {
        x1 = 0;
        y1 = -l.c / l.b;
        x2 = 1;
        y2 = -(l.a + l.c) / l.b;
    }
    Dot a(x1, y1);
    Dot b(x2, y2);
    Vector ab(a, b);
    Vector ac(a, d);
    return std::abs(vectorProductMod(ab, ac) / getDistance(a, b));
}

double getDistance(const Line& l, const Dot& d) { return getDistance(d, l); }

double getDistance(const Dot& d, const Segment& s)
{
    if (d.liesOnSegment(s)) return 0;
    Vector ab(Dot(s.x1, s.y1), Dot(s.x2, s.y2));
    Vector ba = -ab;
    Vector ac(Dot(s.x1, s.y1), d);
    Vector bc(Dot(s.x2, s.y2), d);
    if (scalarProduct(ab, ac) * scalarProduct(ba, bc) > 0)
    {
        Line l(s);
        return getDistance(l, d);
    }
    return std::min(getDistance(d, Dot(s.x1, s.y1)), 
        getDistance(d, Dot(s.x2, s.y2)));
}

double getDistance(const Segment& s,const Dot& d) { return getDistance(d, s); }

double getDistance(const Line& l, const Segment& s)
{
    if (intersect(l ,s)) return 0;
    return std::min(getDistance(Dot(s.x1, s.y1), l), 
        getDistance(Dot(s.x2, s.y2), l));
}

double getDistance(const Segment& s, const Line& l) 
{ 
    return getDistance(l, s); 
}
