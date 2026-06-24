#pragma once

#include <thread>
#include <memory>

#include "runtime/timer/scheduler.hpp"
#include "runtime/task/scheduler.hpp"

namespace exe::runtime::multi_thread
{

class TimerThread : public timer::IScheduler
{
public:
    explicit TimerThread(task::IScheduler* scheduler);
    void set(timer::Duration delay, timer::Handler handler) override;
    void start();
    void stop();

private:
    task::IScheduler* taskScheduler;
    std::thread thread_;
    timer::Handler handler_;
    std::function<void()> threadRoutine;
    timer::Duration delay_;
};

} // namespace exe::runtime::multithread

