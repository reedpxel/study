#pragma once

#include <chrono>

#include "runtime/task/scheduler.hpp"
#include "runtime/timer/scheduler.hpp"
#include "../coroutine/coroutine.hpp"
#include "fiber/core/body.hpp"
#include "runtime/view.hpp"
#include "runtime/submit_task.hpp"
#include "runtime/set_timer.hpp"

namespace exe::fiber
{

class Fiber
{
    friend void go(runtime::View view, Body body);
    friend void go(Body body);
    friend void sleepFor(std::chrono::milliseconds delay);
    friend void yield();

private:
    Fiber(runtime::View& view, Body body);
    void resume();
    static Fiber& self();

private:
    runtime::View view_;
    Coroutine coro_;
    std::chrono::milliseconds delayBeforeResume;

private:
    static thread_local Fiber* self_;
};

} // namespace exe::fiber

