#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>

#define THREAD_COUNT 10

class CyclicBarrier
{
public:
    CyclicBarrier(size_t expected)
            : participants(expected)
            , notBlocked(expected)
    {}

    CyclicBarrier(const CyclicBarrier&) = delete;
    CyclicBarrier& operator=(const CyclicBarrier&) = delete;

    void arriveAndWait()
    {
        std::unique_lock lock_(mutex_);
        --notBlocked;
        if (notBlocked == 0)
        {
            notBlocked = participants;
            cv.notify_all();
        }
        else
        {
            cv.wait(lock_);
        }
    }

private:
    std::condition_variable cv;
    std::mutex mutex_;
    size_t participants;
    size_t notBlocked;
};

int main()
{
    CyclicBarrier barrier_{THREAD_COUNT};
    std::vector<std::thread> threads;
    for (size_t i = 0; i < THREAD_COUNT; ++i)
    {
        threads.emplace_back([&barrier_, i]
        {
            usleep(i * 100'000);
            std::cout << "Thread " << i << " reached the barrier" << std::endl;
            barrier_.arriveAndWait();
            std::cout << "Thread " << i << " passed the barrier" << std::endl;
            if (i == 5)
            {
                usleep(500'000);
            }
            barrier_.arriveAndWait();
            std::cout << "Thread " << i << " passed the barrier again" <<
                std::endl;
        });
    }
    for (auto& th : threads)
    {
        th.join();
    }
}
