#include "wait_group.hpp"

WaitGroup::WaitGroup(size_t count /*= 0*/)
        : count_(count)
{}

void WaitGroup::add(size_t addToCount /*= 1*/)
{
    std::lock_guard guard(mutex_);
    count_ += addToCount;
}

void WaitGroup::done()
{
    std::lock_guard guard(mutex_);
    --count_;
    if (count_ == 0)
    {
        countIsZero.notify_all();
    }
}

void WaitGroup::wait()
{
    std::unique_lock lock_(mutex_);
    countIsZero.wait(lock_, [this]() -> bool { return count_ == 0; });
}

