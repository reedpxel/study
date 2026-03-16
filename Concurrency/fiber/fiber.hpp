// TODO: be able to pass std::function in fiber ctor
// TODO: add exception handling
#ifndef FIBER_H
#define FIBER_H
#pragma once

#include "execution_context.hpp"
#include "scheduler.hpp"

#include <deque>
#include <cassert>
#include <cstring>
#include <memory>
#include <cstdio>

class Scheduler;
[[noreturn]] void trampoline() noexcept;

class FiberObject
{
    friend class Scheduler;
    friend class Fiber;
    friend void trampoline() noexcept;

public:
    enum State
    {
        NotLaunched = 0,
        Runnable,
        Yielded,
        Terminated
    };

    FiberObject(void(*f)()) noexcept;
    ~FiberObject();

    size_t getId() const noexcept;
    State getState() const noexcept;
    void setState(State state_) noexcept;
    void switchToScheduler() noexcept;

private:
    size_t getNextFiberId() const noexcept;
    void setupTrampoline() noexcept;

private:
    void(*routine)();
    size_t id;
    char* stackPtr;
    ExecutionContext context;
    State state;
    bool detachCalled;
    bool routineCompleted;

private:
    static size_t maxFiberId;
    static const size_t STACK_SIZE = 1024 * 1024;
};

[[noreturn]] void trampoline() noexcept;

namespace ThisFiber
{
size_t getId() noexcept;
void yield() noexcept;
};

struct Fiber
{
    constexpr Fiber() noexcept = default;
    Fiber(void(*routine)());
    Fiber(const Fiber&) = delete;
    Fiber& operator=(const Fiber&) = delete;
    Fiber(Fiber&& other) noexcept;
    Fiber& operator=(Fiber&& other) noexcept;
    ~Fiber();

    void swap(Fiber& other) noexcept;
    bool joinable() const noexcept;
    size_t getId() const noexcept;
    void detach();
    void join();
    std::strong_ordering operator<=>(const Fiber& other) const noexcept;

    struct FiberError : public std::exception
    {
        ~FiberError() override;
        const char* what() const noexcept override;
    };

private:
    FiberObject* fiber = nullptr;
};

void swap(Fiber& l, Fiber& r) noexcept;
bool hasReadyFibers() noexcept;

#endif

