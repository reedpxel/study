#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "parallelogramm.h"

class Rectangle : public virtual Parallelogramm
{
protected:
    void rectangleCheck(const Segment& s1, const Segment& s2) const;
    Rectangle(const std::vector<Dot>& dots, bool);
public:
    Rectangle(const std::vector<Dot>& dots);
    Rectangle(const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4);
    double area() const override;
    double perimeter() const override;
    bool containsDot(const Dot& dot) const override;
    ~Rectangle() override;
};

#endif