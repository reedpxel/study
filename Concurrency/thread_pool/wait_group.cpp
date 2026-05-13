#include "wait_group.hpp"

void WaitGroup::add(size_t addToCount /*= 1*/)
{
    std::lock_guard guard(mutex_);
    count += addToCount;
}

void WaitGroup::done()
{
    std::lock_guard guard(mutex_);
    --count;
    if (count == 0)
    {
        countIsZero.notify_all();
    }
}

void WaitGroup::wait()
{
    std::unique_lock lock_(mutex_);
    countIsZero.wait(lock_, [this]() -> bool { return count == 0; });
}

