#include "variant.hpp"

#include <iostream>
#include <variant>

int main()
{
    variant<int, double, std::string> var(5);
    std::cout << get<int>(var) << '\n';
}
