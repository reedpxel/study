#include "table.hpp"
#include <chrono>
#include <iostream>
#include <thread>

#define SEATS 5
#define DURATION_MS 200

using namespace std::chrono;

void measure_time(void(*f)())
{
    auto t1 = high_resolution_clock::now();
    f();
    auto t2 = high_resolution_clock::now();
    auto res = duration_cast<milliseconds>(t2 - t1);
    std::cout << res.count() << " ms\n";
}

void test()
{
    std::vector<std::thread> threads;
    Table table{SEATS};
    [[maybe_unused]] auto philosopherRoutine = [&table](size_t i)
    {
        auto startTime = high_resolution_clock::now();
        for (auto currentTime = high_resolution_clock::now();
            duration_cast<milliseconds>(currentTime - startTime).count() 
                < DURATION_MS;
            currentTime = high_resolution_clock::now())
        {
            table.getPhilosopher(i).eat();
            table.getPhilosopher(i).think();
        }
    };
    for (size_t i = 0; i < SEATS; ++i)
    {
        threads.emplace_back(philosopherRoutine, i);
    }
    for (auto& th : threads)
    {
        th.join();
    }
    for (size_t i = 0; i < SEATS; ++i)
    {
        size_t mealsTaken_ = table.getPhilosopher(i).mealsTaken();
        assert(mealsTaken_ != 0);
        std::cout << mealsTaken_ << ' ';
    }
    std::cout << '\n';
}

int main()
{
    measure_time(test);
}
