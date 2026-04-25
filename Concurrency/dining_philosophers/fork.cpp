#include "fork.hpp"

void Fork::lock()
{
    std::mutex::lock();
    owner = std::this_thread::get_id();
}

bool Fork::try_lock()
{
    if (std::mutex::try_lock())
    {
        owner = std::this_thread::get_id();
        return true;
    }
    return false;
}

void Fork::unlock()
{
    owner = std::thread::id{};
    std::mutex::unlock();
}

bool Fork::lockedByThisThread()
{
    return std::this_thread::get_id() == owner;
}

