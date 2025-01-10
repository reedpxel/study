#include "atomic.h"

class Spinlock
{
    Atomic state = 0; // 0 - unlocked, 1 - locked
public:
    void lock();
    bool try_lock();
    void unlock();
};