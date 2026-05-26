#include <iostream>
#include <vector>
#include <mutex>
#include <thread>

#include "exe.hpp"

using namespace utils;

void test1()
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

void manualLoopUnitTest1()
{
    ManualLoop ml;
    assert(ml.empty());
    std::string str;
    utils::submitTask(ml, [&str] 
        {
            str.push_back('2');
            Fiber::yield();
            str.push_back('4');
        });
    assert(!ml.empty());
    utils::submitTask(ml, [&str] 
        {
            str.push_back('3');
            Fiber::yield();
            str.push_back('5');
        });
    str.push_back('1');
    ml.runTasks();
    assert(ml.empty());
    str.push_back('6');
    assert(str == "123456");
    std::cout << "manualLoopUnitTest passed" << std::endl;
}

void manualLoopUnitTest2()
{
    ManualLoop ml;
    std::vector<int> odds, evens;
    utils::submitTask(ml, [&evens] 
        {
            for (int i = 0; ; i += 2)
            {
                evens.push_back(i);
                Fiber::yield();
            }
        });
    utils::submitTask(ml, [&odds] 
        {
            for (int i = 1; ; i += 2)
            {
                odds.push_back(i);
                Fiber::yield();
            }
        });
    ml.runAtMostTasks(2);
    assert(odds.size() == 1 && odds.front() == 1);
    assert(evens.size() == 1 && evens.front() == 0);
    assert(!ml.empty());
    ml.runAtMostTasks(8);
    assert(odds.size() == 5 && odds.front() == 1);
    assert(evens.size() == 5 && evens.front() == 0);
    for (size_t i = 1; i < odds.size(); ++i)
    {
        assert(odds[i] == odds[i - 1] + 2);
        assert(evens[i] == evens[i - 1] + 2);
    }
    assert(!ml.empty());
    std::cout << "manualLoopUnitTest2 passed" << std::endl;
}

void runAllTests()
{
    test1();
    manualLoopUnitTest1();
    manualLoopUnitTest2();
}

int main()
{
    runAllTests();
}

