#include <type_traits>
#include <functional>
#include "fiber.hpp"

//void* operator new(size_t n)
//{
//    std::cout << n << " bytes allocated\n";
//    return malloc(n);
//}
//
//void operator delete(void* ptr, size_t n) noexcept
//{
//    std::cout << n << " bytes deallocated\n";
//    free(ptr);
//}

int main()
{
    Fiber([] { std::cout << 2 << std::endl; }).detach();
    std::cout << 1 << std::endl;
    ThisFiber::yield();
    std::cout << 3 << std::endl;
}

//int main()
//{
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
//    std::cout << "!!!1!!!";
//}

