#include "this_coro.hpp"

void ThisCoro::suspend(Coroutine* thisCoroutine)
{
    thisCoroutine->calleeContext.switchTo(thisCoroutine->callerContext);
}

[[noreturn]] void trampoline()
{
    // taking pointer to current coroutine that was laid in coroutine ctor
    Coroutine* thisCoroutine = 
        *reinterpret_cast<Coroutine**>(
            reinterpret_cast<char*>(getCurrentContextRbp()) + 8); 
    try
    {
        thisCoroutine->routine_(thisCoroutine);
    }
    catch (...)
    {
        std::terminate();
    }
    thisCoroutine->done_ = true;
    thisCoroutine->calleeContext.switchTo(thisCoroutine->callerContext);
    assert(false);
}

