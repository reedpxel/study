#include "fiber/sched/sched.hpp"

namespace exe::fiber
{

void go(runtime::View view, Body body)
{
    // TODO: wrap new/delete this in shared_ptr
    Fiber* fiber = new Fiber{runtime::tasks(view), body};
    runtime::submitTask(view, [fiber] { fiber->resume();});
}

void go(Body body)
{
    runtime::task::IScheduler& currentScheduler = Fiber::self().scheduler_;
    Fiber* fiber = new Fiber{currentScheduler, body};
    currentScheduler.submit([fiber] { fiber->resume(); });
}

void sleepFor(std::chrono::milliseconds /*delay*/)
{

}

void yield()
{
    ThisCoro::suspend(&Fiber::self().coro_);
}

} // namespace exe::fiber

