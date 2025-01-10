#include "shapes/square.h"

#include <iostream>

int main()
{
    std::vector<Dot> v = {{0, 0}, {5, 0}, {0, 6}, {5, 6}};
    Square sq(v);
    auto v2 = sq.getVertices();
    for (auto& d : v2) std::cout << d.x << '_' << d.y << ' ';
    std::cout << '\n';
    std::cout << sq.area() << ' ' << sq.perimeter();
}
