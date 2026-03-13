// TODO: pass arguments to callable object
// TODO: allocate fiber stack with allocator (template parameter of ctor)
// TODO: move all methods' definitions out of the class
// TODO: launch parameter
// TODO: make getter and setter for fiber state
#ifndef FIBER_H
#define FIBER_H
#pragma once

#include "execution_context.hpp"
#include "scheduler.hpp"

#include <iostream>
#include <deque>
#include <cassert>
#include <cstring>

class Scheduler;

class Fiber
{
public:
    enum State
    {
        NotLaunched = 0,
        Runnable,
        Yielded,
        Terminated
    };

    /*TODO: add constexpr */Fiber() noexcept;
    Fiber(void(*f)()) noexcept;
    Fiber(const Fiber&) = delete;
    Fiber& operator=(const Fiber&) = delete;
    Fiber(Fiber&& other);
    Fiber& operator=(Fiber&& other); // noexcept?
    ~Fiber();
    void swap(Fiber& other) noexcept;
    bool joinable() const noexcept;
    size_t getId() const noexcept;
    void detach(); // const? noexcept?
    void join(); // const? noexcept?
    std::strong_ordering operator<=>(const Fiber& other) const noexcept;

    struct FiberError : public std::exception
    {
        ~FiberError() override;
        const char* what() const noexcept override;
    };
public: // TODO: make private
    size_t getNextFiberId() const noexcept;
    void setupTrampoline() noexcept;
//    /*[[noreturn]]*/ void trampoline(ExecutionContext current, 
//        ExecutionContext next) noexcept;
    void switchToScheduler() noexcept;

public: // TODO: make private
    void(*routine)();
    State state;
    size_t id;
    char* stackPtr;
    char* stackTopPtr;
    ExecutionContext context;
    bool detachCalled; // мб превратится в enum FiberState

//    static std::deque<size_t> schedulerQueue;
    static size_t maxFiberId;
public: // TODO: make private
    static const size_t STACK_SIZE = 1024 * 1024;
};

[[noreturn]] void trampoline() noexcept;

namespace ThisFiber
{
size_t getId() noexcept;
void yield() noexcept;
//void sleepUntil(...);
//void sleepFor(...);
};

void swap(Fiber& l, Fiber& r) noexcept;
bool hasReadyFibers() noexcept;

#endif

