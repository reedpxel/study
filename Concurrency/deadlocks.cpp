#include <iostream>
#include <thread>
#include <mutex>
#include <unistd.h>

void deadlockWithOneNewThread()
{
    std::mutex mutex_;
    mutex_.lock();
    std::cout << "Reachable text in main() thread" << std::endl;
    std::thread new_thread([&mutex_]
    {
        std::cout << "Reachable text in new_thread" << std::endl;
        mutex_.lock();
        std::cout << "Unreachable text in new_thread" << std::endl;
        mutex_.unlock();
    });
    new_thread.join();
    std::cout << "Unreachable text in main() thread" << std::endl;
    mutex_.unlock();
}

void deadlockWithTwoNewThreads()
{
    std::mutex mutex1, mutex2;
    auto l1 = [&mutex1, &mutex2] 
    {
        for (int i = 0; i < 10000; ++i)
        {
            std::lock_guard guard1(mutex1);
            std::lock_guard guard2(mutex2);
            std::cout << "th1 is not locked at iteration " << i << std::endl;
        }
        sleep(1);
        system("clear");
    };
    auto l2 = [&mutex1, &mutex2]
    {
        for (int i = 0; i < 10000; ++i)
        {
            std::lock_guard guard2(mutex2);
            std::lock_guard guard1(mutex1);
            std::cout << "th2 is not locked at iteration " << i << std::endl;
        }
        sleep(1);
        system("clear");
    };

    std::thread(l1).join();
    std::thread(l1).join();
    std::thread(l2).join();
    std::thread(l2).join();
    std::cout << "No deadlock when only one thread launched" << std::endl;
    sleep(1);
    system("clear");
    
    std::thread th1(l1);
    std::thread th2(l2);
    th1.join();
    th2.join();
    std::cout << "Test failed\n";
}

int main()
{
//    deadlockWithOneNewThread();
    deadlockWithTwoNewThreads();
}
