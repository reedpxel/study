#include "coroutine.hpp"

class Coroutine;

namespace ThisCoro
{
    void suspend(Coroutine* thisCoroutine);
}

[[noreturn]] void trampoline();

