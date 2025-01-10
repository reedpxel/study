#include "spinlock.h"

void Spinlock::lock()
{
    state.store(1);
    while (state.exchange(1));
}

bool Spinlock::try_lock()
{
    if (state.load()) return false;
    lock();
    return true;
}

void Spinlock::unlock()
{
    state.store(0);
}