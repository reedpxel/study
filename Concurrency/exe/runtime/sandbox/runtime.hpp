#pragma once

#include "runtime/sandbox/clock.hpp"
#include "runtime/sandbox/timer_queue.hpp"
#include "runtime/sandbox/manual_loop.hpp"
#include "runtime/view.hpp"
#include "runtime/timer/duration.hpp"

namespace exe::runtime::sandbox
{

class Runtime
{
public:
    Runtime();

    operator View()
    {
        return View{&manualLoop_, &timerQueue_};
    }

    size_t runAtMostTasks(size_t limit);

    bool runNextTask()
    {
        return runAtMostTasks(1) == 1;
    }

    size_t runTasks();
    size_t advanceClockBy(timer::Duration delta);
    size_t advanceClockToNextDeadline();
    bool empty();

private:
    ManualLoop manualLoop_;
    TimerQueue timerQueue_;
    Clock clock_;
};

} // namespace exe::runtime::sandbox

