#include "runtime/multi_thread/runtime.hpp"

namespace exe::runtime::multi_thread
{

Runtime::Runtime(size_t workers)
        : tp{workers}
        , timerThreadQueue{}
        , timerThread{}
{}

Runtime& Runtime::withTimers()
{
    timerThread = std::move(std::thread{[this] 
    { 
        timerThreadRoutine(); 
    }});
    return *this;
}

void Runtime::start()
{
    tp.start();
}

void Runtime::stop()
{
    tp.stop();
    timerThreadQueue.close();
    if (timerThread.joinable())
    {
        timerThread.detach();
    }
}

bool Runtime::here()
{
    return tp.current() != nullptr;
}

void Runtime::timerThreadRoutine()
{
    while (true)
    {
        std::optional<TimerThread*> tt = timerThreadQueue.take();
        if (!tt.has_value())
        {
            return;
        }
        tt.value()->start();
    }
}

} // namespace exe::runtime::multi_thread

