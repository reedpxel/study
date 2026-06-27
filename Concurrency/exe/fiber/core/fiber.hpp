#pragma once

#include <chrono>

#include "runtime/task/scheduler.hpp"
#include "../coroutine/coroutine.hpp"
#include "fiber/core/body.hpp"
#include "runtime/view.hpp"

namespace exe::fiber
{

class Fiber
{
    friend void go(runtime::View view, Body body);
    friend void go(Body body);
    friend void sleepFor(std::chrono::milliseconds delay);
    friend void yield();

private:
    Fiber(runtime::task::IScheduler& scheduler, Body body);
    void resume();
    static Fiber& self();

private:
    runtime::task::IScheduler& scheduler_;
    Coroutine coro_;

private:
    static thread_local Fiber* self_;
};

} // namespace exe::fiber

