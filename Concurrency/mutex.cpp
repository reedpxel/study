#include <iostream>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <atomic>

#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

#define UNLOCKED 0
#define LOCKED 1

using namespace std::chrono_literals;

// Заблокировать поток целиком в пользовательском режиме невозможно.

// Когда поток заходит в заблокированную секцию, планировщик ОС должен
// переключить этот поток на другой (а не крутиться в while(true);, как в
// спинлоке). Это делается при помощи фьютексов - объектов, которые в своих
// методах делают системные вызовы в ядро. В Linux управляются через системный
// вызов futex()


// Почему такой мьютекс не работает?
// Пусть поток 1 зашел в lock(), в нем прошел мимо FUTEX_WAIT, некоторые остальные залочились. Затем 
// поток 1 вышел из критической секции, вызвал unlock() (то есть, присвоил lock_
// значение UNLOCKED и после вызова FUTEX_WAKE один из заблокированных потоков
// (поток 2) разлочился и
// начал выполнять код в критической секции). Теперь, часть потоков залочена, но
// значение lock_ = UNLOCKED. Таким образом, какой-нибудь поток (поток 3) из не
// заблокированных во время пребывания потока 1 в критической секции может дойти до
// вызова lock(), значение lock_ = UNLOCKED, он пропустит FUTEX_WAIT и войдет в
// критическую секцию. Атомарность нарушена, поскольку потоки 2 и 3 одновременно
// в критической секции.
/*
class Mutex
{
public:
    std::atomic<int> lock_ = UNLOCKED;

public:
    void lock();
    void unlock();
};

void Mutex::lock()
{
    int unlocked = UNLOCKED;
    while (!std::atomic_compare_exchange_strong(&lock_, &unlocked, LOCKED))
    {
        syscall(SYS_futex, reinterpret_cast<int*>(&lock_), FUTEX_WAIT, 1,
                NULL, NULL, 0);
    }
}

void Mutex::unlock()
{
    lock_ = UNLOCKED;
    syscall(SYS_futex, reinterpret_cast<int*>(&lock_), FUTEX_WAKE, 1,
            NULL, NULL, 0);
}
*/

/* atomic_compare_exchange_strong(obj, exp, des) - возвращает obj == exp
    - если они равны, записывает в obj значение des
    - если нет, записывает в exp значение obj
*/

class Mutex
{
    std::atomic<int> state = 0;
    
    int compare_exchange(std::atomic<int>* state, int expected, int desired);
public:
    void lock();
    void unlock();
};

int Mutex::compare_exchange(std::atomic<int>* state, int expected, int
        desired)
{
    int* pExpected = &expected;
    std::atomic_compare_exchange_strong(state, pExpected, desired);
    return *pExpected;
}

void Mutex::lock()
{
    int c = compare_exchange(&state, 0, 1);
    if (c)
    {
        do
        {
            if (c == 2 || compare_exchange(&state, 1, 2))
            {
                syscall(SYS_futex, reinterpret_cast<int*>(&state), FUTEX_WAIT, 
                    2, NULL, NULL, 0);
            }
        } while (c = compare_exchange(&state, 0, 2));
    }
}

void Mutex::unlock()
{
    if (state.fetch_sub(1) != 1)
    {
        state.store(0);
        syscall(SYS_futex, reinterpret_cast<int*>(&state), FUTEX_WAKE, 1, NULL,
            NULL, 0);
    }
}

class Stack
{
    int buffer[100];
    int top_ = 0;
//    Mutex mutex_;
//    std::mutex mutex_;

public:
    void push(int value)
    {
//        mutex_.lock();
        *(buffer + top_) = value;
        ++top_;
//        mutex_.unlock();
    }
    void pop()
    {
//        mutex_.lock();
        --top_;
//        mutex_.unlock();
    }
    int top()
    {
        return *(buffer + top_ - 1);
    }
    int size() { return top_; }
};

int main()
{/*
    Stack s;
    for (int i = 0; i < 50; ++i) s.push(i);
    std::cout << s.size() << ' ';
    size_t n = 10000;
    Mutex mutex_;
    auto f = [&s, n, &mutex_]()
    {
        for(int i = 0; i < n; ++i)
        {
//            mutex_.lock();
            s.push(i);
            s.pop();
//            mutex_.unlock();
        }
    };
    std::vector<std::thread> th;
    for (int i = 0; i < 4; ++i)
    {
        th.emplace_back(f);
    }
    for (auto& th_ : th) th_.join();
    std::cout << s.size() << '\n';
*/
    // int futex_obj = 0;
    // std::thread th1([&futex_obj]()
    // {
    //     std::cout << "thread 1 starts and locks itself\n";
    //     syscall(SYS_futex, &futex_obj, FUTEX_WAIT, 
    //         0, NULL, NULL, 0);
    //     std::cout << "thread 1 unlocked\n";        
    // });
    // std::thread th2([&futex_obj]()
    // {
    //     std::cout << "thread 2 starts\n";
    //     std::this_thread::sleep_for(2s);    
    //     std::cout << "thread 2 unlocks thread 1\n";
    //     syscall(SYS_futex, reinterpret_cast<int*>(&futex_obj), FUTEX_WAKE, LOCKED,
    //         NULL, NULL, 0);
    // });
    // th1.join();
    // th2.join();
    /*
    Mutex mutex_;
    std::thread th1([&mutex_]()
    {
        mutex_.lock();
        while (true)
        {
            std::cout << "th1 is not blocked\n";
            std::this_thread::sleep_for(1s);
            mutex_.unlock();
        }
    });
    std::thread th2([&mutex_]()
    {
        mutex_.lock();
        while (true)
        {
            std::cout << "th2 is not blocked\n";
            std::this_thread::sleep_for(1s);
        }
    });
    th1.join();
    th2.join();
    */
    size_t count = 0;
    Mutex mutex_;
    auto f = [&count, &mutex_]()
    {
        for(int i = 0; i < 25000; ++i)
        {
            mutex_.lock();
            ++count;
            mutex_.unlock();
        }
    };
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back(f);
    }
    for(auto& thread : threads) thread.join();
    std::cout << count;
}
