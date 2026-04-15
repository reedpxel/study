#ifndef FORK_H
#define FORK_H
#pragma once

#include <thread>
#include <mutex>
#include <atomic>

class Fork : public std::mutex
{
public:
    void lock();
    bool try_lock();
    void unlock();
    bool lockedByThisThread();

private:
    std::atomic<std::thread::id> owner{};
};

#endif // FORK_H

