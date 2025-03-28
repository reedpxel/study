#include "variant.hpp"

#include <iostream>
#include <variant>

struct Talkative
{
    int x;

    Talkative(int x) : x(x) { std::cout << "Talkative(int)\n"; }
    Talkative(const Talkative&) 
    { 
        std::cout << "Talkative(const Talkative&)\n"; 
    }
    Talkative(Talkative&&)
    {
        std::cout << "Talkative(Talkative&&)\n";
    }
    ~Talkative() { std::cout << "~Talkative()\n"; }
};

int main()
{
    variant<int, double, std::string, Talkative> var;
    var = 5;
    std::cout << get<int>(var) << '\n';
    var = 1.123;
    std::cout << get<double>(var) << '\n';
    var = Talkative(1);
    var = "abcdaaaaaaaaaaaaaaaa";
    std::cout << get<std::string>(var) << '\n';
    try
    {
        get<int>(var);
    } catch (const std::exception& e) {
        std::cout << e.what();
    }
}
