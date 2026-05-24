#include <iostream>
#include <vector>
#include <mutex>
#include <thread>

#include "exe.hpp"

struct F
{
    WaitGroup& wg_;
    const int i_;
    mutable int count = 0;

    F(WaitGroup& wg, int i)
            : wg_(wg)
            , i_(i)
    {}

    void operator()() const
    {
        for (int i = 0; i < 10; ++i)
        {
            std::cout << i_;
        }
        wg_.done();
    }
};

int main()
{
    ThreadPool tp;
    WaitGroup wg{4};
    tp.start();
    for (int i = 0; i < 4; ++i)
    {
        F f{wg, i};
        Fiber::go(tp, f);
    }
    wg.wait();
    std::cout << std::endl;
    wg.add(4);
    Fiber::go(tp, [&wg]
        {
            for (int i = 0; i < 10; ++i)
            {
                std::cout << 0;
                Fiber::yield();
            }
            wg.done();
        });
    Fiber::go(tp, [&wg]
        {
            for (int i = 0; i < 10; ++i)
            {
                std::cout << 1;
                Fiber::yield();
            }
            wg.done();
        });
    Fiber::go(tp, [&wg]
        {
            for (int i = 0; i < 10; ++i)
            {
                std::cout << 2;
                Fiber::yield();
            }
            wg.done();
        });
    Fiber::go(tp, [&wg]
        {
            for (int i = 0; i < 10; ++i)
            {
                std::cout << 3;
                Fiber::yield();
            }
            wg.done();
        });

    wg.wait();
    tp.stop();
}

