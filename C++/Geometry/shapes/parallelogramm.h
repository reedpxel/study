#ifndef PARALLELOGRAMM_H
#define PARALLELOGRAMM_H
#include "trapezium.h"

class Parallelogramm : public Trapezium
{
protected:
    void parallelogrammCheck(const Segment& s1, const Segment& s2, 
        const Segment& s3, const Segment& s4) const;
    Parallelogramm(const std::vector<Dot>& dots, bool);
public:
    Parallelogramm(const std::vector<Dot>& dots);
    Parallelogramm(const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4);
    double area() const override;
    double perimeter() const override;
    bool containsDot(const Dot& dot) const override;
    ~Parallelogramm() override;
};

#endif