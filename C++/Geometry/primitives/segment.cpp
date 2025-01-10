#include "segment.h"

Segment::Segment(double x1, double y1, double x2, double y2) 
        : x1(x1)
        , y1(y1)
        , x2(x2)
        , y2(y2) 
    {}

Segment::Segment(const Dot& d1, const Dot& d2) 
        : x1(d1.x)
        , y1(d1.y)
        , x2(d2.x)
        , y2(d2.y) 
    {}

bool Segment::contains(const Dot& d) const { return d.liesOnSegment(*this); }

double Segment::length() const 
{ 
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool intersect(const Segment& s1, const Segment& s2)
{
    Dot a(s1.x1, s1.y1);
    Dot b(s1.x2, s1.y2);
    Dot c(s2.x1, s2.y1);
    Dot d(s2.x2, s2.y2);
    Vector ab(a, b);
    Vector ac(a, c);
    Vector ad(a, d);
    Vector ca = -ac;
    Vector cd(c, d);
    Vector cb(c, b);
    double p1 = vectorProductMod(ab, ac);
    double p2 = vectorProductMod(ab, ad);
    double p3 = vectorProductMod(cd, ca);
    double p4 = vectorProductMod(cd, cb);
    if (!p1 && !p2 && !p3 && !p4) // если отрезки лежат на одной линии, нужна 
                                  // доп. проверка
    {
        if (a == c || a == d || b == c || b == d) return true;
        if ((std::max(a.x, b.x) < std::min(c.x, d.x)) || 
            (std::max(a.y, b.y) < std::min(c.y, d.y)) || 
            (std::min(a.x, b.x) > std::max(c.x, d.x)) || 
            (std::min(a.y, b.y) > std::max(c.y, d.y))) 
        {
            return false;
        }
        return true;
    }
    return vectorProductMod(ab, ac) * vectorProductMod(ab, ad) <= 0 &&
        vectorProductMod(cd, ca) * vectorProductMod(cd, cb) <= 0;
}

Dot intersectionDot(const Segment& s1, const Segment& s2)
{
    if (!intersect(s1, s2)) throw noIntersection();
    return intersectionDot(Line(s1), Line(s2));
}

bool intersect(const Segment& s, const Line& l)
{
    Dot d1(0, -l.c / l.b);
    Dot d2(1, -(l.a + l.c) / l.b);
    Vector ab(d1, d2);
    Vector ac(d1, Dot(s.x1, s.y1));
    Vector ad(d1, Dot(s.x2, s.y2));
    return vectorProductMod(ab, ac) * vectorProductMod(ab, ad) <= 0;
}

bool intersect(const Line& l, const Segment& s) { return intersect(s, l); }

Dot intersectionDot(const Segment& s, const Line& l)
{
    if (!intersect(s, l)) throw noIntersection();
    return intersectionDot(Line(s), l);
}

Dot intersectionDot(const Line& l, const Segment& s)
{ 
    return intersectionDot(s, l); 
}

void f(const noIntersection&) {}
