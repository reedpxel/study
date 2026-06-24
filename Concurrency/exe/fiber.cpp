#include "fiber.hpp"

namespace exe
{

void Fiber::go(IScheduler& scheduler, const Task& body)
{
    // TODO: wrap new/delete this in shared_ptr
    Fiber* fiber = new Fiber{scheduler, body};
    scheduler.submit([fiber] { fiber->resume(); });
}

void Fiber::go(const std::function<void()>& body)
{
    IScheduler& currentScheduler = Fiber::self().scheduler_;
    Fiber* fiber = new Fiber{currentScheduler, body};
    currentScheduler.submit([fiber] { fiber->resume(); });
}

void Fiber::yield()
{
    ThisCoro::suspend(&Fiber::self().coro_);
}

Fiber::Fiber(IScheduler& scheduler, const std::function<void()>& body)
        : scheduler_(scheduler)
        , coro_([body](Coroutine*) { body(); })
{}

void Fiber::resume()
{
    self_ = this;
    coro_.resume();
    if (coro_.isDone())
    {
        delete this;
    } 
    else
    {
        scheduler_.submit([this] { this->resume(); });
    }
}

Fiber& Fiber::self()
{
    return *self_;
}

thread_local Fiber* Fiber::self_ = nullptr;

} // namespace exe

