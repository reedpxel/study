#include "atomic.h"
#include "spinlock.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <climits>
#include <chrono>

using namespace std::chrono_literals;

int main()
{
    Spinlock spinlock_;
    std::thread th1([&spinlock_]()
    {
        std::cout << "th1 locked\n";
        spinlock_.try_lock();
        std::cout << "th1 unlocked\n";
    });
    std::thread th2([&spinlock_]()
    {
        std::this_thread::sleep_for(2s);
        spinlock_.try_lock();
        spinlock_.unlock();
    });
    th1.join();
    th2.join();
}