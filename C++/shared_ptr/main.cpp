#include "shared_ptr.hpp"

#include <iostream>
#include <memory>

int main()
{
    shared_ptr<std::string> shptr1(new std::string("1"));
    shptr1.reset(new std::string("2"));
    std::cout << shptr1 << std::endl;
}

