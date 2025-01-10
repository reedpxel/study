#ifndef LINE_H
#define LINE_H
#include "dot.h"
#include "segment.h"
#include "vector.h"
#include "../exceptions.h"

class Dot;
class Segment;
class Line;

struct Line
{
    double a;
    double b;
    double c;
    Line(double a, double b, double c);
    Line(const Dot& d1, const Dot& d2);
    Line(const Segment& s);
    bool contains(const Dot& d) const;
    bool operator==(const Line& l) const;
};

bool intersect(const Line& l1, const Line& l2);
Dot intersectionDot(const Line& l1, const Line& l2);

#endif