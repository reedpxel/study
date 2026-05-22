#include "coroutine.hpp"

Coroutine::Coroutine(const std::function<void(Coroutine*)>& routine)
        : routine_(routine)
        , stackPtr_(malloc(STACK_SIZE))
        , callerContext{}
        // Stack grows from bigger addresses to smaller ones
        , calleeContext{reinterpret_cast<void*>(
            reinterpret_cast<char*>(stackPtr_) 
            + STACK_SIZE 
            - (ExecutionContext::CALLEE_SAVED_REGISTERS_ON_STACK + 2) * 8
            )}
        , done_(false)
{
    setupTrampoline();
}

Coroutine::~Coroutine()
{
    free(stackPtr_);
}

void Coroutine::resume()
{
    callerContext.switchTo(calleeContext);
}

bool Coroutine::isDone()
{
    return done_;
}

void Coroutine::setupTrampoline()
{
    // placing pointer to this coroutine object on top of the coroutine
    // stack to be used to call routine_
    Coroutine** stackTop = reinterpret_cast<Coroutine**>(
        reinterpret_cast<char*>(stackPtr_) + STACK_SIZE - 8);
    *stackTop = this;

    // placing trampoline after "this" to be called after context switch
    void(**ripAfterContextSwitch)() = 
        reinterpret_cast<void(**)()>(
            reinterpret_cast<char*>(stackPtr_) + STACK_SIZE - 16);
    *ripAfterContextSwitch = &trampoline;
}

