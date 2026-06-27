#include "runtime/multi_thread/timer_thread.hpp"

namespace exe::runtime::multi_thread
{

TimerThread::TimerThread(task::IScheduler* taskScheduler)
        : taskScheduler_(taskScheduler)
        , timerStartingThread{}
        , timerThreadRoutine([this] 
        {
            while (!stopped)
            {
                std::optional<std::function<void()>> task = timerQueue.take();
                if (task == std::nullopt)
                {
                    return;
                }
                try
                {
                    std::thread{[task = std::move(task)] 
                    {
                        task.value()(); 
                    }}.detach();
                }
                catch (...)
                {
                    std::terminate();
                }
            }
        })
        , timerQueue{}
        , stopped(false)
{}

void TimerThread::set(timer::Duration delay, timer::Handler handler)
{
    timerQueue.put([this, delay, handler = std::move(handler)] 
    {
        std::this_thread::sleep_for(delay);
        taskScheduler_->submit(handler);
    });
}

void TimerThread::start()
{
    timerStartingThread = std::move(std::thread{timerThreadRoutine});
}

void TimerThread::stop()
{
    timerQueue.close();
    timerStartingThread.detach();
}

} // namespace exe::runtime::multi_thread

