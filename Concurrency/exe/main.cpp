#include <iostream>
#include <vector>
#include <mutex>
#include <thread>

#include "exe.hpp"

int main()
{
    ThreadPool tp;
    tp.start();
    WaitGroup wg{2 * std::thread::hardware_concurrency()};
    
    for (size_t i = 0; i < 2 * std::thread::hardware_concurrency(); ++i)
    {
        Fiber::go(tp, [&wg, i]
            {
                for (int j = 0; j < 10; ++j)
                {
                    std::cout << i;
                }
                wg.done();
            });
    }
    wg.wait();
    std::cout << std::endl;

    wg.add(2 * std::thread::hardware_concurrency());
    for (size_t i = 0; i < 2 * std::thread::hardware_concurrency(); ++i)
    {
        Fiber::go(tp, [&wg, i]
            {
                for (int j = 0; j < 10; ++j)
                {
                    std::cout << i;
                    Fiber::yield();
                }
                wg.done();
            });
    }
    wg.wait();
    std::cout << std::endl;

    tp.stop();
}

