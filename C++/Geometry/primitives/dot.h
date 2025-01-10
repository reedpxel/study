#ifndef DOT_H
#define DOT_H
#include "line.h"
#include "segment.h"
#include "vector.h"
#include "../exceptions.h"
#include <cmath>

struct Line;
struct Segment;
struct Vector;

struct Dot
{
    double x;
    double y;
    bool operator<(const Dot& other) const;
    bool operator==(const Dot& other) const;
    bool operator!=(const Dot& other) const;
    Dot(double x, double y);
    bool liesOnLine(const Line& l) const;
    bool liesOnSegment(const Segment& s) const;
};

double getDistance(const Dot& d1, const Dot& d2);
double getDistance(const Line& l1, const Line& l2);
double getDistance(const Segment& s1, const Segment& s2);
double getDistance(const Dot& d, const Line& l);
double getDistance(const Line& l, const Dot& d);
double getDistance(const Dot& d, const Segment& s);
double getDistance(const Segment& s,const Dot& d);
double getDistance(const Line& l, const Segment& s);
double getDistance(const Segment& s, const Line& l);

#endif