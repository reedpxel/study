#include "weak_ptr.hpp"

#include <iostream>

int main()
{
    shared_ptr<std::string> shptr(new std::string("abcd"));
    weak_ptr<std::string> wptr = shptr;
    weak_ptr<std::string> wptr2;
    std::cout << wptr2.use_count() << ' ' << wptr.use_count() << ' ';
    shared_ptr<std::string> shptr2 = wptr.lock();
    std::cout << wptr.use_count() << std::endl;
    auto shptr3 = wptr2.lock();
    std::cout << 
        shptr.owner_before(shptr2) << ' ' <<
        shptr.owner_before(wptr) << ' ' <<
        wptr2.owner_before(wptr) << ' ' << 
        wptr2.owner_before(shptr3) << std::endl; // 1 1 0 1
}

//int main()
//{
//    {
//        shared_ptr<std::string> shptr;
//        weak_ptr<std::string> wptr = shptr;
//        weak_ptr<std::string> wptr2;
//        wptr = wptr2;
//    }
//    std::cout << "Test 1 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr(new std::string("abc"));
//        weak_ptr<std::string> wptr = shptr;
//        shptr.reset();
//        weak_ptr<std::string> wptr2;
//        wptr2 = shptr;
//        weak_ptr<std::string> wptr3;
//        wptr3 = wptr;
//        weak_ptr<std::string> wptr4;
//        wptr4 = std::move(wptr2);
//        weak_ptr<std::string> wptr5;
//    }
//    std::cout << "Test 2 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr = make_shared<std::string>("abc");
//        auto shptr2 = shptr;
//        weak_ptr<std::string> wptr = shptr;
//        shptr.reset();
//        weak_ptr<std::string> wptr2;
//        wptr2 = shptr;
//        weak_ptr<std::string> wptr3;
//        wptr3 = wptr;
//        weak_ptr<std::string> wptr4;
//        wptr4 = std::move(wptr2);
//        weak_ptr<std::string> wptr5;
//    }
//    std::cout << "Test 3 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr(new std::string("abcd"));
//        shared_ptr<std::string> shptr2 = shptr;
//        weak_ptr<std::string> wptr = shptr;
//        shptr.reset();
//        weak_ptr<std::string> wptr2;
//        wptr2 = shptr;
//        weak_ptr<std::string> wptr3;
//        wptr3 = wptr;
//        weak_ptr<std::string> wptr4;
//        wptr4 = std::move(wptr2);
//        weak_ptr<std::string> wptr5;
//    }
//    std::cout << "Test 4 passed" << std::endl;
//}

//int main()
//{
//    {
//        shared_ptr<std::string> shptr;
//        weak_ptr<std::string> wptr = shptr;
//    }
//    std::cout << "Test 1 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr(new std::string("abcde"));
//        weak_ptr<std::string> wptr = shptr;
//        shptr.reset();
//    }
//    std::cout << "Test 2 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr = make_shared<std::string>("abcd");
//        weak_ptr<std::string> wptr1 = shptr;
//        weak_ptr<std::string> wptr2 = shptr;
//        weak_ptr<std::string> wptr3 = wptr1;
//        shptr.reset();
//    }
//    std::cout << "Test 3 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr1(new std::string("abccd"));
//        shared_ptr<std::string> shptr2 = shptr1;
//        weak_ptr<std::string> wptr1 = shptr1;
//        weak_ptr<std::string> wptr2 = wptr1;
//    }
//    std::cout << "Test 4 passed" << std::endl;
//}
//
