#ifndef FIBER_H
#define FIBER_H

#include "schedulers/i_scheduler.hpp"
#include "../coroutine/coroutine.hpp"
#include "utils/utils.hpp"

namespace utils
{
void submitTask(IScheduler& scheduler, const Task& task);
}

class Fiber
{
    friend void utils::submitTask(IScheduler& scheduler, const Task& task);

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

#endif // FIBER_H

