#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <future>

#include "thread_pool.hpp"
#include "wait_group.hpp"

void test1()
{
    UnboundedBlockingMPMCQueue<std::string> queue_;
    std::vector<std::thread> threads;
    for (int i = 0; i < 20; ++i)
    {
        threads.emplace_back([&queue_] 
            {
                for (int i = 0; i < 10'000; ++i)
                {
                    queue_.put("abcd");
                    queue_.take();
                }
            });
    }
    for (auto& th : threads)
    {
        th.join();
    }
}

void test2()
{
    UnboundedBlockingMPMCQueue<std::string> queue_;
    std::vector<std::thread> threads;
    for (int i = 0; i < 20; ++i)
    {
        threads.emplace_back([&queue_] 
            {
                for (int i = 0; i < 1000; ++i)
                {
                    queue_.put("abcd");
                }
            });
    }
    for (auto& th : threads)
    {
        th.join();
    }

    queue_.close();
    threads.clear();

    for (int i = 0; i < 20; ++i)
    {
        threads.emplace_back([&queue_] 
            {
                for (int i = 0; i < 1000; ++i)
                {
                    queue_.put("abcd");
                    queue_.take();
                }
            });
    }
    for (auto& th : threads)
    {
        th.join();
    }
    queue_.take();
}

void test3()
{
    std::atomic<size_t> count = 0;
    size_t incrementCount = 100'000;
    ThreadPool tp;
    WaitGroup wg;
    wg.add(incrementCount);
    for (size_t i = 0; i < incrementCount; ++i)
    {
        tp.submit([&count, &wg] 
            {
                ++count;
                wg.done();
            });
    }
    tp.start();
    wg.wait();
    tp.stop();
    assert(count == incrementCount);
}

void test4()
{
    std::atomic<size_t> count = 0;
    size_t incrementCount = 100'000;
    ThreadPool tp;
    WaitGroup wg;
    wg.add(incrementCount);
    std::function<void()> inc;
    inc = [&count, &wg, incrementCount, &inc]
    {
        ++count;
        if (count.load() < incrementCount)
        {
            ThreadPool::current()->submit(inc);
        }
        wg.done();
    };
    for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        tp.submit(inc);
    }
    tp.start();
    wg.wait();
    assert(count.load() == incrementCount);
    tp.stop();
}

void test5()
{
    ThreadPool tp;
    tp.start();
    std::promise<size_t> promise_;
    std::future<size_t> future_ = promise_.get_future();
    tp.submit([&promise_] {
        assert(ThreadPool::current() != nullptr);
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        promise_.set_value(123);
    });
    size_t res = future_.get();
    assert(res == 123);
    tp.stop();
}

void test6()
{
    ThreadPool{};
    assert(ThreadPool::current() == nullptr);
}

void callAllTests()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
}

int main()
{
    callAllTests();
}

