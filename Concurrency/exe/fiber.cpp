#include "fiber.hpp"

void Fiber::go(Scheduler& scheduler, const std::function<void()>& body)
{
    Fiber* fiber = new Fiber{body};
    scheduler.submit([fiber] { fiber->resume(); });
}

void Fiber::go(const std::function<void()>& body)
{
    Fiber* fiber = new Fiber{body};
    Scheduler::current()->submit([fiber] { fiber->resume(); });
}

void Fiber::yield()
{
    ThisCoro::suspend(&Fiber::self().coro_);
}

Fiber& Fiber::self()
{
    return *self_;
}

Fiber::Fiber(const std::function<void()>& body)
        : coro_([&body](Coroutine*) { body(); })
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
        ThreadPool::current()->submit([this] { this->resume(); });
    }
}

thread_local Fiber* Fiber::self_ = nullptr;

