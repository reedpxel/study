#ifndef SQUARE_H
#define SQUARE_H
#include "rectangle.h"
#include "rhombus.h"

class Square : public Rectangle, public Rhombus
{
public:
    Square(const std::vector<Dot>& dots);
    Square(const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4);
    double area() const override;
    double perimeter() const override;
    bool containsDot(const Dot& dot) const override;
    ~Square() override;
};

#endif