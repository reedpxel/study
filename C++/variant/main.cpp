#include "variant.hpp"

#include <iostream>
#include <variant>
#include <vector>

struct Talkative
{
    int x;

    Talkative(int x) : x(x) { std::cout << "Talkative(int)\n"; }
    Talkative(const Talkative&) 
    { 
        std::cout << "Talkative(const Talkative&)\n"; 
    }
    Talkative(Talkative&& other)
    {
        std::cout << "Talkative(Talkative&&)\n";
        x = other.x;
        other.x = 0;
    }
    ~Talkative() { std::cout << "~Talkative()\n"; }
};

int main()
{
    variant<int, std::string, std::vector<int>, Talkative> var1("abc");
    variant<int, std::string, std::vector<int>, Talkative> var2(
        std::vector<int>(10, 10));
    std::cout << "var1: " << var1.index() << ' ' << get<std::string>(var1) <<
        std::endl << "var2: " << var2.index() << ' ';
    for (auto x : get<std::vector<int>>(var2))
    {
        std::cout << x << ' ';
    }
    std::cout << std::endl;
    var1 = std::move(var2);
    std::cout << "var1: " << var1.index() << ' ';
    for (int x : get<std::vector<int>>(var1))
    {
        std::cout << x << ' ';
    }
    std::cout << std::endl;
    std::cout << "var2: " << var2.index() << std::endl;
}

//int main()
//{
//    static_assert(
//        std::is_same_v<int, get_type_by_index_t<0, int, double, char, 
//            std::string, std::vector<int>>>);
//    static_assert(   
//        std::is_same_v<double, get_type_by_index_t<1, int, double, char, 
//            std::string, std::vector<int>>>);
//    static_assert(
//        std::is_same_v<char, get_type_by_index_t<2, int, double, char, 
//            std::string, std::vector<int>>>);
//    static_assert(
//        std::is_same_v<std::string, get_type_by_index_t<3, int, double, char, 
//            std::string, std::vector<int>>>);
//    static_assert(
//        std::is_same_v<std::vector<int>, get_type_by_index_t<4, int, double, 
//            char, std::string, std::vector<int>>>);
//    static_assert(
//        std::is_same_v<void, get_type_by_index_t<100, int, double, char, 
//            std::string, std::vector<int>>>);
//}
