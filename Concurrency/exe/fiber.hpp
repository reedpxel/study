#pragma once

#include "runtime/task/scheduler.hpp"
#include "../coroutine/coroutine.hpp"

namespace exe
{

using namespace runtime::task;

class Fiber
{
public:
    static void go(IScheduler& scheduler, const Task& body);
    static void go(const Task& body);
    static void yield();

private:
    Fiber(IScheduler& scheduler, const Task& body);
    void resume();
    static Fiber& self();

private:
    IScheduler& scheduler_;
    Coroutine coro_;

private:
    static thread_local Fiber* self_;
};

} // namespace exe

