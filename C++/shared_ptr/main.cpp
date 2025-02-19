#include "weak_ptr.hpp"

#include <iostream>

int main()
{
    {
        shared_ptr<std::string> shptr;
        weak_ptr<std::string> wptr = shptr;
    }
    std::cout << "Test 1 passed" << std::endl;
    {
        shared_ptr<std::string> shptr(new std::string("abcde"));
        weak_ptr<std::string> wptr = shptr;
        shptr.reset();
    }
    std::cout << "Test 2 passed" << std::endl;
    {
        shared_ptr<std::string> shptr = make_shared<std::string>("abcd");
        weak_ptr<std::string> wptr1 = shptr;
        weak_ptr<std::string> wptr2 = shptr;
        weak_ptr<std::string> wptr3 = wptr1;
        shptr.reset();
    }
    std::cout << "Test 3 passed" << std::endl;
    {
        shared_ptr<std::string> shptr1(new std::string("abccd"));
        shared_ptr<std::string> shptr2 = shptr1;
        weak_ptr<std::string> wptr1 = shptr1;
        weak_ptr<std::string> wptr2 = wptr1;
    }
    std::cout << "Test 4 passed" << std::endl;
}

