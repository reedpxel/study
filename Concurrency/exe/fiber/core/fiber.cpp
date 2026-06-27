#include "fiber.hpp"

namespace exe::fiber
{

Fiber::Fiber(runtime::task::IScheduler& scheduler, Body body)
        : scheduler_(scheduler)
        , coro_([body = std::move(body)](Coroutine*) { body(); })
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

} // namespace exe::fiber

