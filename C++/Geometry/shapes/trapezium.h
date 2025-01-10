#ifndef TRAPEZIUM_H
#define TRAPEZIUM_H
#include "quadrilateral.h"

class Trapezium : public Quadrilateral
{
protected:
    void trapeziumCheck() const;

    Trapezium(const std::vector<Dot>& dots, bool); 
public:
    Trapezium(const std::vector<Dot>& dots);
    Trapezium(const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4);
    double area() const override;
    double perimeter() const override;
    bool containsDot(const Dot& dot) const override;
    ~Trapezium() override;
};

#endif