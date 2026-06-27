#include <iostream>
#include <vector>
#include <mutex>
#include <thread>

#include "exe.hpp"
#include "runtime/multi_thread/thread_pool.hpp"

using namespace exe;

int main()
{
    runtime::MultiThread mt{8};
    mt.start();
    thread::WaitGroup wg{8};

    for (int i = 0; i < 8; ++i)
    {
        fiber::go(mt, [&wg, i]
        {
            for (int j = 0; j < 10; ++j)
            {
                std::cout << i;
                fiber::yield();
            }
            wg.done();
        });
    }
    wg.wait();
    std::cout << std::endl;

    mt.stop();
}

//int main()
//{
//    runtime::MultiThread mt;
//    mt.withTimers().start();
//    std::cout << mt.here() << std::endl;
//    thread::WaitGroup wg{1};
//    runtime::setTimer(mt, std::chrono::milliseconds{1}, [&wg, &mt] 
//    {
//        std::cout << mt.here() << std::endl;
//        wg.done();
//    });
//    wg.wait();
//    mt.stop();
//}

//int main()
//{
//    runtime::Sandbox sandbox;
//    for (int i = 0; i < 5; ++i)
//    {
//        runtime::setTimer(sandbox, std::chrono::seconds{i}, [i]
//        {
//            std::cout << i << std::endl;
//        });
//    }
//    std::cout << sandbox.advanceClockBy(std::chrono::seconds{5}) << std::endl;
//    sandbox.runTasks();
//}

//int main()
//{
//    int* x = nullptr;
//    std::cout << &x << std::endl;
//    std::cout << std::thread::hardware_concurrency() << std::endl;
//    runtime::MultiThread mt;
//    mt.withTimers().start();
//    while (true)
//    {
//        thread::WaitGroup wg{1};
//        auto t1 = std::chrono::high_resolution_clock::now();
//        runtime::setTimer(mt, std::chrono::milliseconds{1000}, [&wg] 
//        {
//            wg.done();
//        });
//        wg.wait();
//        auto t2 = std::chrono::high_resolution_clock::now();
//        auto res = std::chrono::duration_cast<std::chrono::microseconds>(
//            t2 - t1);
//        std::cout << res.count() << " us\n";
//    }
//    mt.stop();
//}

//int main()
//{
//    runtime::MultiThread mt;
//    mt.start();
//    thread::WaitGroup wg{2 * std::thread::hardware_concurrency()};
// 
//    for (size_t i = 0; i < 2 * std::thread::hardware_concurrency(); ++i)
//    {
//        fiber::go(mt, [&wg, i]
//            {
//                for (int j = 0; j < 10; ++j)
//                {
//                    std::cout << i;
//                }
//                wg.done();
//            });
//    }
//    wg.wait();
//    std::cout << std::endl;
//
//    wg.add(2 * std::thread::hardware_concurrency());
//    for (size_t i = 0; i < 2 * std::thread::hardware_concurrency(); ++i)
//    {
//        fiber::go(mt, [&wg, i]
//            {
//                for (int j = 0; j < 10; ++j)
//                {
//                    std::cout << i;
//                    fiber::yield();
//                }
//                wg.done();
//            });
//    }
//    wg.wait();
//    std::cout << std::endl;
//
//    mt.stop();
//}

//int main()
//{
//    runtime::Sandbox sandbox;
//    assert(sandbox.empty());
//    std::string str;
//    fiber::go(sandbox, [&str] 
//        {
//            str.push_back('2');
//            fiber::yield();
//            str.push_back('4');
//        });
//    assert(!sandbox.empty());
//    fiber::go(sandbox, [&str] 
//        {
//            str.push_back('3');
//            fiber::yield();
//            str.push_back('5');
//        });
//    str.push_back('1');
//    sandbox.runTasks();
//    assert(sandbox.empty());
//    str.push_back('6');
//    assert(str == "123456");
//    std::cout << "test passed" << std::endl; 
//}

//int main()
//{
//    runtime::Sandbox sandbox;
//    std::vector<int> odds, evens;
//    fiber::go(sandbox, [&evens] 
//    {
//        for (int i = 0; ; i += 2)
//        {
//            evens.push_back(i);
//            fiber::yield();
//        }
//    });
//    fiber::go(sandbox, [&odds] 
//    {
//        for (int i = 1; ; i += 2)
//        {
//            odds.push_back(i);
//            fiber::yield();
//        }
//    });
//
//    sandbox.runAtMostTasks(2);
//    assert(odds.size() == 1 && odds.front() == 1);
//    assert(evens.size() == 1 && evens.front() == 0);
//    assert(!sandbox.empty());
//    sandbox.runAtMostTasks(8);
//    assert(odds.size() == 5 && odds.front() == 1);
//    assert(evens.size() == 5 && evens.front() == 0);
//    for (size_t i = 1; i < odds.size(); ++i)
//    {
//        assert(odds[i] == odds[i - 1] + 2);
//        assert(evens[i] == evens[i - 1] + 2);
//    }
//    assert(!sandbox.empty());
//    std::cout << "test passed" << std::endl;
//}
//
