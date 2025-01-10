#include "line.h"

Line::Line(double a, double b, double c) : a(a), b(b), c(c)
{
    if (!a && !b) throw invalidLine{};
}

Line::Line(const Dot& d1, const Dot& d2) : a(0), b(1), c(-d1.y)
{
    if (d1.y != d2.y)
    {
        a = 1;
        b = (d2.x - d1.x) / (d1.y - d2.y);
        c = -b * d1.y - d1.x;
    }
    if (!a && !b) throw invalidLine{};
}

Line::Line(const Segment& s) : a(0), b(1), c(-s.y1)
{
    if (s.y1 != s.y2)
    {
        a = 1;
        b = (s.x2 - s.x1) / (s.y1 - s.y2);
        c = -b * s.y1 - s.x1;
    }
    if (!a && !b) throw invalidLine();
}

bool Line::contains(const Dot& d) const { return d.liesOnLine(*this); }

bool Line::operator==(const Line& other) const
{
    return this->a / this->b == other.a / other.b && this->c == other.c;
}

bool intersect(const Line& l1, const Line& l2)
{
    if (!(l1.a * l2.b - l1.b * l2.a))
    {
        if (l1.c == l2.c) return true;
        return false;
    }
    return true;
}

Dot intersectionDot(const Line& l1, const Line& l2)
{
    double det = l1.a * l2.b - l1.b * l2.a;
    if (!det)
    {
        if (l1.c == l2.c) throw linesAreSame{};
        throw linesAreParallel{};
    }
    double _x, _y;
    if (!l1.a)
    {
        _y = -l1.c / l1.b;
        _x = -(l2.b * _y + l2.c) / l2.a;
    } else if (!l2.a){
        _y = - l2.c / l2.b;
        _x = -(l1.b * _y + l1.c) / l1.a;
    } else if (!l1.b) {
        _x = -l1.c / l1.a;
        _y = -(l2.a * _x + l2.c) / l2.b;
    } else if (!l2.b) {
        _x = - l2.c / l2.a;
        _y = -(l1.a * _x + l1.c) / l1.b;
    } else {
        _y = (l1.c * l2.a - l2.c * l1.a) / det;
        _x = -(l1.b * _y + l1.c) / l1.a;
    }
    return Dot(_x, _y);
}
