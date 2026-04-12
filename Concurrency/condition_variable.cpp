#include <iostream>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <atomic>

class ConditionVariable
{
public:
    template <typename MutexType>
    void wait(std::unique_lock<MutexType>& lock_)
    {
        int tmp = value.load();
        previous.store(tmp);

        lock_.unlock();
        if (syscall(SYS_futex, &value, FUTEX_WAIT, tmp, NULL, NULL, 0) < 0)
        {
            perror("FUTEX_WAIT");
            return;
        }
        lock_.lock();
    }

    void notifyOne()
    {
        // ABA problem: a thread calls wait, if 2^^32 threads call notifyOne 
        // and these 2 instructions get executed before futex_wait, the first 
        // thread will not notice that value and tmp are not equal and will block
        int tmp = previous.load() + 1;
        value.store(tmp);

        if (syscall(SYS_futex, &value, FUTEX_WAKE, 1) < 0)
        {
            perror("FUTEX_WAKE");
        }
    }

    void notifyAll()
    {
        int tmp = previous.load() + 1;
        value.store(tmp);
        
        if (syscall(SYS_futex, &value, FUTEX_WAKE, INT_MAX) < 0)
        {
            perror("FUTEX_WAKE");
        }
    }
private:
    std::atomic<int> value{0};
    std::atomic<unsigned> previous{0}; // unsigned used to avoid UB when the
                                       // value overflows
};

