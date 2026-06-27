#include "runtime/multi_thread/runtime.hpp"

namespace exe::runtime::multi_thread
{

Runtime::Runtime(size_t workers/* = std::thread::hardware_concurrency()*/)
        : threadPool_{workers}
        , timerThread_{&threadPool_}
        , withTimers_{false}
{}

Runtime& Runtime::withTimers()
{
    withTimers_ = true;
    return *this;
}

void Runtime::start()
{
    threadPool_.start();
    if (withTimers_)
    {
        timerThread_.start();
    }
}

void Runtime::stop()
{
    if (withTimers_)
    {
        timerThread_.stop();
    }
    threadPool_.stop();
}

bool Runtime::here()
{
    return threadPool_.current() != nullptr;
}

} // namespace exe::runtime::multi_thread

