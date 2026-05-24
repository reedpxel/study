#ifndef FIBER_H
#define FIBER_H

#include "../thread_pool/thread_pool.hpp"
#include "../coroutine/coroutine.hpp"

class Fiber
{
    using Scheduler = ThreadPool;

public:
    static void go(Scheduler& scheduler, const std::function<void()>& body);
    static void go(const std::function<void()>& body);
    static void yield();
    static Fiber& self();

private:
    Fiber(const std::function<void()>& body);
    void resume();

private:
    Coroutine coro_;

private:
    static thread_local Fiber* self_;
};

#endif // FIBER_H

