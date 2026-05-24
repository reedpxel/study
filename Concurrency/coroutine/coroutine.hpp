#ifndef COROUTINE_H
#define COROUTINE_H

#include <functional>
#include <cassert>
#include <exception>
#include <cstdlib>

#include "execution_context.hpp"
#include "this_coro.hpp"

class Coroutine
{
    friend void trampoline();
    friend void ThisCoro::suspend(Coroutine* self);

public:
    Coroutine(const std::function<void(Coroutine*)>& routine);
    ~Coroutine();
    void resume();
    bool isDone();

private:
    void setupTrampoline();

private:
    std::function<void(Coroutine*)> routine_;
    void* stackPtr_;
    ExecutionContext callerContext;
    ExecutionContext calleeContext;
    bool done_;

private:
    static const size_t STACK_SIZE = 1024 * 1024;
};

#endif // COROUTINE_H

