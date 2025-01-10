#ifndef SHAPE_H
#define SHAPE_H
#include "../primitives/dot.h"
#include "../primitives/line.h"
#include "../primitives/segment.h"
#include "../primitives/vector.h"

class Shape
{
public:
    virtual double area() const = 0;
    virtual double perimeter() const = 0;
    virtual bool containsDot(const Dot& dot) const = 0;
    virtual ~Shape() = default;
};

#endif