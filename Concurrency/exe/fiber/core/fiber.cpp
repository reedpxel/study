#include "fiber.hpp"

namespace exe::fiber
{

Fiber::Fiber(runtime::View& view, Body body)
        : view_(view)
        , coro_([body = std::move(body)](Coroutine*) { body(); })
        , delayBeforeResume{0}
{}

void Fiber::resume()
{
    self_ = this;
    coro_.resume();
    if (coro_.isDone())
    {
        delete this;
    }
    else if (delayBeforeResume != std::chrono::milliseconds{0})
    {
        runtime::setTimer(view_, delayBeforeResume, [this] { resume(); });
    }
    else
    {
        runtime::submitTask(view_, [this] { resume(); });
    }
}

Fiber& Fiber::self()
{
    return *self_;
}

thread_local Fiber* Fiber::self_ = nullptr;

} // namespace exe::fiber

