#include "vector.h"

Vector::Vector(double x, double y) : x(x), y(y) {}

Vector::Vector(const Dot& d1, const Dot& d2) 
        : x(d2.x - d1.x)
        , y(d2.y - d1.y) 
{}

Vector::Vector(const Segment& s)
        : x(s.x2 - s.x1)
        , y(s.y2 - s.y1)
{}

double Vector::mod() const { return sqrt(x * x + y * y); }

bool Vector::operator==(const Vector& other) const
{ 
    return x == other.x && y == other.y; 
}

Vector Vector::operator-() const
{
    Vector res(-x, -y);
    return res;
}

bool areCollinear(const Vector& v1, const Vector& v2)
{
    if ((!v1.x && !v2.x) || (!v1.y && !v2.y)) return true;
    return v1.x / v2.x == v1.y / v2.y;
}

bool areCodirectional(const Vector& v1, const Vector& v2)
{
    if (v1.mod() && v2.mod())
    {
        if (!v1.x && !v2.x)
        {
            if (!v1.mod() || !v2.mod()) return true;
            return v1.y / v2.y > 0;
        }
        return areCollinear(v1, v2) && (v1.x / v2.x > 0);
    }
    return true;
}

double scalarProduct(const Vector& v1, const Vector& v2) 
{ 
    return v1.x * v2.x + v1.y * v2.y; 
}

double vectorProductMod(const Vector& v1, const Vector& v2) 
{ 
    return v1.x * v2.y - v1.y * v2.x; 
}

double getAngle(const Vector& v1, const Vector& v2)
{
    return acos(scalarProduct(v1, v2) / v1.mod() / v2.mod()) * 180 / M_PI;
}
