#include <iostream>
#include <thread>
#include <atomic>
#include <vector>

class Spinlock
{
public:
    void lock()
    {
        while (locked.exchange(true));
    }

    bool try_lock()
    {
        if (!locked.exchange(true))
        {
            return true;
        }
        return false;
    }

    void unlock()
    {
        locked.store(false);
    }
private:
    std::atomic<bool> locked = false;
};

int main()
{
    Spinlock spinlock_;
    std::vector<std::thread> threads;
    size_t x = 0;
    for (int i = 0; i < 100'000; ++i)
    {
        threads.emplace_back([&x, &spinlock_] 
        {
            spinlock_.lock();
            ++x;
            spinlock_.unlock();
        });
    }
    for (auto& th : threads)
    {
        th.join();
    }
    std::cout << x << std::endl;
}

