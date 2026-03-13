#include <type_traits>
#include "fiber.hpp"

void f() { std::cout << "abcd\n"; }

int main()
{
    scheduler.printSchedulerQueue();
    Fiber f1([]
    {
        std::cout << "!!! 2 !!!" << std::endl;
    });
    scheduler.printSchedulerQueue();
    f1.detach();
    std::cout << "!!! 1 !!!" << std::endl;
    ThisFiber::yield();
    scheduler.printSchedulerQueue();
    std::cout << "!!! 3 !!!" << std::endl;
}

