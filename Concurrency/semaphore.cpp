#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#define THREAD_COUNT 100
#define ITERATIONS 100'000

class Semaphore
{
public:
    Semaphore(size_t permits)
            : count(permits)
    {}
    
    void acquire()
    {
        std::unique_lock lock_(mutex_);
        while (count == 0)
        {
            cv.wait(lock_);
        }
        --count;
    }

    void release()
    {
        std::unique_lock lock_(mutex_);
        ++count;
        if (count == 1)
        {
            cv.notify_one();
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv;
    size_t count;
};

void semaphoreTest()
{
    Semaphore semaphore{1};
    size_t x = 0;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < THREAD_COUNT; ++i)
    {
        threads.emplace_back([&x, &semaphore]
        {
            for (size_t i = 0; i < ITERATIONS; ++i)
            {
                semaphore.acquire();
                ++x;
                semaphore.release();
            }
        });
    }
    for (auto& th : threads)
    {
        th.join();
    }
    std::cout << x << std::endl;
}

void measure_time(void(*f)())
{
    auto t1 = std::chrono::high_resolution_clock::now();
    f();
    auto t2 = std::chrono::high_resolution_clock::now();
    auto res = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << res.count() << " ms\n";
}

int main()
{
    while (true)
    {
        measure_time(semaphoreTest);
    }
}
