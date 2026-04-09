#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

// TODO: memory orders in atomic operations

using namespace std::chrono_literals;

#define THREAD_COUNT 10
#define CYCLE_COUNT 100

std::atomic<size_t> lockCycleCount = 0;

inline void pause()
{
    asm volatile("pause\n":::"memory"); // volatile forces the compiler not to
                                        // omit inserted asm instructions
}

class BadSpinlock
{
public:
    void lock()
    {
        while (locked.exchange(true))
        {
            pause();
        }
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

class ABitBetterSpinlock
{
public:
    void lock()
    {
        while (locked.exchange(true))
        {
            while (locked.load())
            {
                pause();
            }
        }
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

class TicketLock
{
public:
    void lock()
    {
        const size_t this_thread_ticket = next_free_ticket.fetch_add(1);
        while (this_thread_ticket != owner_ticket.load())
        {
            pause();
        }
    }

    bool try_lock()
    {
        size_t tmp = next_free_ticket.load();
        return next_free_ticket.compare_exchange_strong(tmp, tmp + 1);
    }

    void unlock()
    {
        owner_ticket += 1;
    }

private:
    alignas(std::hardware_destructive_interference_size) 
        std::atomic<size_t> owner_ticket = 0;
    alignas(std::hardware_destructive_interference_size) 
        std::atomic<size_t> next_free_ticket = 0;
};

// does not work effectively if wait queue is not empty
class QueueSpinlock
{
    friend class Guard;
public:
    QueueSpinlock() = default;

    class Guard
    {
        friend class QueueSpinlock;
    public:
        explicit Guard(QueueSpinlock& spinlock_)
                : spinlock_(spinlock_)
        {
            spinlock_.lock(this);
        }

        ~Guard()
        {
            spinlock_.unlock(this);
        }
    private:
        QueueSpinlock& spinlock_;
        alignas(std::hardware_destructive_interference_size) 
            std::atomic<bool> owns{false};
        Guard* next = nullptr;
    };
private:
    void lock(Guard* guard)
    {
        Guard* prevTail = tail.exchange(guard);
        if (prevTail != nullptr)
        {
            // wait queue is not empty, lock is not free
            prevTail->next = guard;
            while (!guard->owns.load())
            {
                pause();
            }
        }
    }

    void unlock(Guard* guard)
    {
        if (tail.compare_exchange_strong(guard, nullptr))
        {
            return;
        }
        while (guard->next == nullptr)
        {
            pause();
        }
        guard->next->owns.store(true);
    }

    alignas(std::hardware_destructive_interference_size) 
        std::atomic<Guard*> tail{nullptr};
};

template <typename SpinlockType> 
void spinlock_test()
{
    SpinlockType spinlock_;
    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        threads.emplace_back([&spinlock_, i] 
        {
            for (int j = 0; j < CYCLE_COUNT; ++j)
            {
                spinlock_.lock();
                spinlock_.unlock();
            }
        });
    }
    for (auto& th : threads)
    {
        th.join();
    }
}

void queueSpinlockTest()
{
    QueueSpinlock spinlock_;
    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        threads.emplace_back([&spinlock_, i] 
        {
            for (int j = 0; j < CYCLE_COUNT; ++j)
            {
                QueueSpinlock::Guard guard(spinlock_);
            }
        });
    }
    for (auto& th : threads)
    {
        th.join();
    }
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
        measure_time(spinlock_test<BadSpinlock>);
        measure_time(spinlock_test<ABitBetterSpinlock>);
        measure_time(queueSpinlockTest);
        std::cout << std::endl;
    }
}

