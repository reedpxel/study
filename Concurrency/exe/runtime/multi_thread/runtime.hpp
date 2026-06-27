#pragma once

#include <thread>
#include <vector>

#include "runtime/view.hpp"
#include "runtime/multi_thread/thread_pool.hpp"
#include "runtime/multi_thread/timer_thread.hpp"

namespace exe::runtime::multi_thread
{

template <typename T>
struct Debug
{
    Debug(T) = delete;
};

class Runtime
{
public:
    explicit Runtime(size_t workers = std::thread::hardware_concurrency());

    operator View()
    {
        return View{&threadPool_, &timerThread_};
    }

    Runtime& withTimers();
    void start();
    void stop();
    bool here();

private:
    ThreadPool threadPool_;
    TimerThread timerThread_;
    bool withTimers_;
};

} // namespace exe::runtime::multi_thread

