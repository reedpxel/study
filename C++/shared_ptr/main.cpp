#include "shared_ptr.hpp"

#include <iostream>
#include <memory>

int main()
{
    shared_ptr<int> shptr1(new int(5));
    std::cout << static_cast<bool>(shptr1) << '\n';
    shptr1.reset();
    std::cout << static_cast<bool>(shptr1) << '\n';
}

