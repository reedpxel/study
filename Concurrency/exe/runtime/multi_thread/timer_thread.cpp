#include "runtime/multi_thread/timer_thread.hpp"

namespace exe::runtime::multi_thread
{

TimerThread::TimerThread(task::IScheduler* taskScheduler)
        : taskScheduler(taskScheduler)
        , thread_{}
        , handler_{}
        , threadRoutine{}
        , delay_{}
{}

void TimerThread::set(timer::Duration delay, timer::Handler handler)
{
    delay_ = delay;
    handler_ = std::move(handler);
    threadRoutine = [this]
    {
        std::this_thread::sleep_for(delay_);
        taskScheduler->submit(handler_);
    };
}

void TimerThread::start()
{
    thread_ = std::move(std::thread{threadRoutine});
    thread_.detach();
}

void TimerThread::stop()
{
}

} // namespace exe::runtime::multi_thread

