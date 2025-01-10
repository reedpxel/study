#ifndef RHOMBUS_H
#define RHOMBUS_H
#include "parallelogramm.h"

class Rhombus : public virtual Parallelogramm
{
protected:
    void rhombusCheck(const Segment& s1, const Segment& s2) const;
    Rhombus(const std::vector<Dot>& dots, bool);
public:
    Rhombus(const std::vector<Dot>& dots);
    Rhombus(const Dot& d1, const Dot& d2, const Dot& d3, const Dot& d4);
    double area() const override;
    double perimeter() const override;
    bool containsDot(const Dot& dot) const override;
    ~Rhombus() override;
};

#endif