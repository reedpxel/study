#include <iostream>
#include <vector>
#include <thread>
#include <cstdint>
#include <atomic>
#include <condition_variable>

#define THREAD_COUNT 100
#define INCREMENT_COUNT 100'000

class WaitGroup
{
public:
    void add(size_t addToCount = 1)
    {
        std::lock_guard guard(mutex_);
        count += addToCount;
    }

    void done()
    {
        std::lock_guard guard(mutex_);
        --count;
        if (count == 0)
        {
            countIsZero.notify_all();
        }
    }

    void wait()
    {
        std::unique_lock lock_(mutex_);
        countIsZero.wait(lock_, [this]() -> bool { return count == 0; });
    }

private:
    size_t count = 0;
    std::condition_variable countIsZero;
    std::mutex mutex_;
};

int main()
{
    std::vector<std::thread> threads;
    WaitGroup wg;
    wg.add(THREAD_COUNT);
    std::atomic<size_t> uselessCount = 0;
    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        threads.emplace_back([&wg, &uselessCount]
        {
            for (int i = 0; i < INCREMENT_COUNT; ++i)
            {
                ++uselessCount;
            }
            wg.done();
        });
        threads.back().detach();
    }
    wg.wait();
    std::cout << uselessCount << std::endl;
}

