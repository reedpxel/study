#ifndef SEGMENT_H
#define SEGMENT_H
#include "dot.h"
#include "line.h"
#include "vector.h"
#include "../exceptions.h"

struct Dot;
struct Line;
struct Vector;

struct Segment
{
    double x1;
    double y1;
    double x2;
    double y2;
    Segment(double x1, double y1, double x2, double y2);
    Segment(const Dot& d1, const Dot& d2);
    bool contains(const Dot& d) const;
    double length() const;
};

void f(const noIntersection&);
bool intersect(const Segment& s1, const Segment& s2);
Dot intersectionDot(const Segment& s1, const Segment& s2);
bool intersect(const Segment& s, const Line& l);
bool intersect(const Line& l, const Segment& s);
Dot intersectionDot(const Segment& s, const Line& l);
Dot intersectionDot(const Line& l, const Segment& s);

#endif