#include "convex_polygon.h"

ConvexPolygon::ConvexPolygon(const std::vector<Dot>& dots, bool)
        : pDots(new std::vector<Dot>(dots))
{}

std::vector<Dot> ConvexPolygon::getConvexEnvelope(
    std::vector<Dot> vertices_) const
{
    std::set<Dot> checkSameDots(vertices_.begin(), vertices_.end());
    vertices_ = std::vector(checkSameDots.begin(), checkSameDots.end());
    if (vertices_.size() < 3) throw invalidDots("Too few dots\n");
    std::vector<Dot> res;
    std::sort(vertices_.begin(), vertices_.end(), [](Dot& a, Dot& b)
    {
        return a.y == b.y ? a.x < b.x : a.y < b.y;
    });
    res.push_back(vertices_[0]);
    Vector v(1, 0);
    Dot found_vertice = vertices_[1];
    double angle = 180;
    while (found_vertice != vertices_[0]) 
    {
        for (auto _dot : vertices_)
        {
            if (_dot == res.back()) continue;
            double an = getAngle(v, Vector(res.back(), _dot));
            if (an < angle || 
                (an == angle && getDistance(res.back(), _dot) >
                getDistance(res.back(), found_vertice)))
            {
                found_vertice = _dot;
                angle = an;
            }
        }
        v = Vector(res.back(), found_vertice);
        res.push_back(found_vertice);
        angle = 180;
    }
    res.pop_back();
    return res;
}

void ConvexPolygon::ctorHelper(const Dot& dot)
{
    if ((*pDots).size() < 2) throw invalidDots("Too few dots\n");
    (*pDots).push_back(dot);
}

ConvexPolygon::ConvexPolygon(const std::vector<Dot>& dots) 
        : pDots(new std::vector<Dot>(getConvexEnvelope(dots)))
{}

std::vector<Dot> ConvexPolygon::getVertices() const { return *pDots; }

double ConvexPolygon::area() const
{
    double res = 0;
    for (int i = 0; i < (*pDots).size(); ++i)
    {
        res += vectorProductMod(Vector(Dot(0, 0), (*pDots)[i]), 
            Vector(Dot(0, 0), (*pDots)[(i + 1) % (*pDots).size()])) / 2;
    }
    return res;
}

double ConvexPolygon::perimeter() const
{
    double res = 0;
    for (int i = 0; i < (*pDots).size(); ++i)
    {
        res += getDistance((*pDots)[i], (*pDots)[(i + 1) % (*pDots).size()]);
    }
    return res;
}

bool ConvexPolygon::containsDot(const Dot& dot) const
{
    for (int i = 0; i < (*pDots).size(); ++i)
    {
        if ((*pDots)[i] == dot) return true;
    }
    std::vector<Segment> edges;
    edges.reserve((*pDots).size());
    for (int i = 0; i < (*pDots).size() - 1; ++i)
    {
        edges.push_back(Segment((*pDots)[i], (*pDots)[i + 1]));
    }
    edges.push_back(Segment((*pDots).back(), (*pDots).front()));
    for (int i = 0; i < (*pDots).size(); ++i)
    {
        if (edges[i].contains(dot)) return true;
    }
    Segment check_segment(dot, 
        Dot(std::numeric_limits<double>::max(), dot.y));
    std::vector<bool> intersects((*pDots).size(), false);
    size_t amount_of_intersections = 0;
    int j = (*pDots).size() - 1;
    for (int i = 0; i < (*pDots).size(); ++i)
    {
        if (Line((*pDots)[i], (*pDots)[(i + 1) % (*pDots).size()]) == 
            Line(check_segment)) 
        {
            continue;
        }
        if (intersect(check_segment, edges[i]))
        {
            intersects[i] = true;
            if ((*pDots)[i].y == dot.y)
            {
                if (((*pDots)[j].y - dot.y) * 
                    ((*pDots)[(i + 1) % (*pDots).size()].y - dot.y) < 0)
                {
                    intersects[i] = false;
                }
            }
        }
        j = (j + 1) % (*pDots).size();
    }
    for (auto x : intersects)
    {
        if (x) ++amount_of_intersections;
    }
    return amount_of_intersections % 2;
}

ConvexPolygon::~ConvexPolygon() {}
