#include <iostream>
#include <vector>
#include <thread>
#include <cstdint>
#include <atomic>
#include <condition_variable>

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
            cv.notify_all();
        }
    }

    void wait()
    {
        std::unique_lock lock_(mutex_);
        cv.wait(lock_);
    }

private:
    size_t count = 0;
    std::condition_variable cv;
    std::mutex mutex_;
};

int main()
{
    std::vector<std::thread> threads;
    WaitGroup wg;
    std::atomic<size_t> uselessCount = 0;
    for (int i = 0; i < 100; ++i)
    {
        threads.emplace_back([&wg, &uselessCount]
        {
            wg.add();
            for (int i = 0; i < 100'000; ++i)
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

