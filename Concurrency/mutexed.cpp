#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

template <typename T, typename MutexType>
class Mutexed;

template <typename T, typename MutexType = std::mutex>
struct OwnerRef
{
    OwnerRef(Mutexed<T, MutexType>* mutexedPtr)
            : mutexedPtr(mutexedPtr)
    {}

    OwnerRef(const OwnerRef&) = delete;
    OwnerRef& operator=(const OwnerRef&) = delete;
    OwnerRef(OwnerRef&&) = delete;
    OwnerRef& operator=(OwnerRef&&) = delete;

    ~OwnerRef()
    {
        mutexedPtr->mutex_.unlock();
    }

    T* operator->()
    {
        return &mutexedPtr->obj;
    }

    T& operator*()
    {
        return mutexedPtr->obj;
    }

private:
    Mutexed<T, MutexType>* mutexedPtr;
};

template <typename T, typename MutexType = std::mutex>
class Mutexed
{
    friend class OwnerRef<T, MutexType>;
public:
    template <typename... Args>
    Mutexed(Args&&... args)
            : obj(std::forward<Args>(args)...)
    {}

    OwnerRef<T, MutexType> acquire()
    {
        mutex_.lock();
        return OwnerRef<T, MutexType>{this};
    }

private:
    T obj;
    MutexType mutex_;
};

template <typename T, typename MutexType = std::mutex>
OwnerRef<T, MutexType> acquire(Mutexed<T, MutexType>& mutexed)
{
    return mutexed.acquire();
}

int main()
{
    Mutexed<size_t> x{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 100'000; ++i)
    {
        threads.emplace_back([&x] 
        {
            ++*acquire(x);
        });
    }
    for (auto& th : threads)
    {
        th.join();
    }
    auto ref = x.acquire();
    std::cout << *ref << std::endl;
}

