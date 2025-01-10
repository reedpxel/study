#ifndef VECTOR_H
#define VECTOR_H
#include "dot.h"
#include "../exceptions.h"
#include <cmath>

struct Dot;
struct Line;
struct Segment;

struct Vector
{
    double x;
    double y;
    Vector(double x, double y);
    Vector(const Dot& d1, const Dot& d2);
    Vector(const Segment& s);
    double mod() const;
    bool operator==(const Vector& other) const;
    Vector operator-() const;
};

bool areCollinear(const Vector& v1, const Vector& v2);
bool areCodirectional(const Vector& v1, const Vector& v2);
double scalarProduct(const Vector& v1, const Vector& v2);
double vectorProductMod(const Vector& v1, const Vector& v2);
double getAngle(const Vector& v1, const Vector& v2);

#endif