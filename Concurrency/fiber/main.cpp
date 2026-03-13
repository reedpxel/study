#include <type_traits>
#include <functional>
#include "fiber.hpp"

int x = 0;

int main()
{
    Fiber f1([]
    {
        while (true)
        {
            ++x;
            std::cout << x;
            if (x > 1 || x < 0)
            {
                return;
            }
            ThisFiber::yield();
        }
    });
    f1.detach();
    Fiber f2([]
    {
        while (true)
        {
            --x;
            std::cout << x;
            if (x > 1 || x < 0)
            {
                return;
            }
            ThisFiber::yield();
        }
    });
    f2.detach();
}

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
//    std::cout << "!!!1!!!";
//}

//int main()
//{
//    Fiber f1([] 
//    {
//        for (int i = 0; i < 10; ++i)
//        {
//            std::cout << i;
//            if (i % 2)
//            {
//                ThisFiber::yield();
//            }
//        }
//    });
//    Fiber f2([]
//    {
//        for (int i = 0; i < 10; ++i)
//        {
//            std::cout << i;
//            if (i % 2)
//            {
//                ThisFiber::yield();
//            }
//        }
//    });
//    f1.detach();
//    f2.detach();
//    for (int i = 0; i < 10; ++i)
//    {
//        std::cout << i;
//        if (i % 2)
//        {
//            ThisFiber::yield();
//        }
//    }
//}

//int main()
//{
//    scheduler.printSchedulerQueue();
//    Fiber f1([]
//    {
//        std::cout << "!!! 2 !!!" << std::endl;
//    });
//    scheduler.printSchedulerQueue();
//    f1.detach();
//    std::cout << "!!! 1 !!!" << std::endl;
//    ThisFiber::yield();
//    scheduler.printSchedulerQueue();
//    std::cout << "!!! 3 !!!" << std::endl;
//}

