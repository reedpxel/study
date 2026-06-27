#include "fiber/sched/sched.hpp"

namespace exe::fiber
{

void go(runtime::View view, Body body)
{
    // TODO: wrap new/delete this in shared_ptr
    Fiber* fiber = new Fiber{view, body};
    runtime::submitTask(view, [fiber] { fiber->resume();});
}

void go(Body body)
{
    runtime::View& currentView = Fiber::self().view_;
    Fiber* fiber = new Fiber{currentView, body};
    runtime::submitTask(currentView, [fiber] { fiber->resume(); });
}

void sleepFor(std::chrono::milliseconds delay)
{
    Fiber::self().delayBeforeResume = delay;
    ThisCoro::suspend(&Fiber::self().coro_);
}

void yield()
{
    Fiber::self().delayBeforeResume = std::chrono::milliseconds{0};
    ThisCoro::suspend(&Fiber::self().coro_);
}

} // namespace exe::fiber

