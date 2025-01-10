#include "unordered_map.hpp"

#include <iostream>

int main()
{
    [[maybe_unused]] unordered_map<int, int> uMap;
    std::cout << "works\n";
    int* ptr = new int(123);
    std::cout << ptr << std::endl;
    delete ptr;
}
