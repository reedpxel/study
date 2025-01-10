#ifndef CONVEX_POLYGON_H
#define CONVEX_POLYGON_H
#include "shape.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <memory>
#include <set>

class ConvexPolygon : public Shape
{
protected:
    // a pointer to vector that stores dots of a polygon and its inheritors
    std::unique_ptr<std::vector<Dot>> pDots;
    
    // a constructor for inheritors that simply initializes pDots with vector 
    // of dots without any checks
    ConvexPolygon(const std::vector<Dot>& dots, bool);
    std::vector<Dot> getConvexEnvelope(std::vector<Dot> vertices_) const;

    // All template methods must be defined in header files, because compiler 
    // creates a new class for each case of using a method with various types
template <typename... Dots>
    void ctorHelper(const Dot& dot, const Dots&... dots)
    {
        (*pDots).push_back(dot);
        ctorHelper(dots...);
    }
    void ctorHelper(const Dot& dot);
public:
    struct invalidDots : public std::logic_error 
    {
        using std::logic_error::logic_error;
    };

    // public constructors
    template <typename... Dots>
    ConvexPolygon(const Dot& dot, const Dots&... dots)
            : pDots(new std::vector<Dot>())
    {
        ctorHelper(dot, dots...);
        *pDots = getConvexEnvelope(*pDots);
    }
    ConvexPolygon(const std::vector<Dot>& dots);

    // public methods
    std::vector<Dot> getVertices() const;
    double area() const override;
    double perimeter() const override;
    bool containsDot(const Dot& dot) const override;

    ~ConvexPolygon() override;
};

#endif