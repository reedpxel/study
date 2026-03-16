#include <type_traits>
#include <functional>
#include <iostream>
#include "fiber.hpp"

int main()
{
    Fiber([]
    {
        std::cout << 2;
        ThisFiber::yield();
        std::cout << 4;
    }).detach();
    std::cout << 1;
    ThisFiber::yield();
    std::cout << 3;
    Fiber([] { std::cout << 5; }).detach();
    ThisFiber::yield();
    std::cout << 6;
}

//int main()
//{
//    Fiber([] { std::cout << "abcd" << std::endl; }).detach();
//}

//int main()
//{
//    Fiber([] {}).join();
//}

//int main()
//{
//    Fiber([]
//    {
//        int x = 0;
//        for (int i = 0; i < 10; ++i)
//        {
//            std::cout << x;
//            ++x;
//            ThisFiber::yield();
//        }
//    }).detach();
//    Fiber([]
//    {
//        int x = 0;
//        for (int i = 0; i < 10; ++i)
//        {
//            std::cout << x << std::endl;
//            ++x;
//            ThisFiber::yield();
//        }
//    }).detach();
//}

//int x = 0;
//
//int main()
//{
//    Fiber([]
//    {
//        while (true)
//        {
//            ++x;
//            std::cout << x;
//            if (x > 1 || x < 0)
//            {
//                return;
//            }
//            ThisFiber::yield();
//        }
//    }).detach();
//    Fiber([]
//    {
//        while (true)
//        {
//            --x;
//            std::cout << x;
//            if (x > 1 || x < 0)
//            {
//                return;
//            }
//            ThisFiber::yield();
//        }
//    }).detach();
//}

//int main()
//{
//    Fiber f2([] 
//    {
//        std::cout << "!!!2!!!\n";
//        Fiber f3([] 
//        { 
//            std::cout << "!!!3!!!\n";
//            Fiber f4([] { std::cout << "!!!4!!!\n"; });
//            f4.detach();
//        });
//        f3.detach();
//    });
//    f2.detach();
//    std::cout << "!!!1!!!\n";
//}

//int main()
//{
//    Fiber([] 
//    {
//        try
//        {
//            throw 1;
//        }
//        catch (...)
//        {
//            std::cout << "Caught\n";
//        }
//    }).detach();
//}

