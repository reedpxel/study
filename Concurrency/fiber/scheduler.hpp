#ifndef SCHEDULER_H
#define SCHEDULER_H
#pragma once

#include "fiber.hpp"
#include "execution_context.hpp"

#include <deque>
#include <memory>
#include <iostream>

int main();
class FiberObject;

class Scheduler
{
    friend void schedulerRunLoopTrampoline() noexcept;

public:
    Scheduler();
    ~Scheduler();
    void pushFiber(FiberObject* fiber);
    void terminateCurrentFiber() noexcept;
    FiberObject* getCurrentFiber() const noexcept;
    ExecutionContext* getContext() noexcept;
    bool hasReadyFibers() const noexcept;
private:
    void runLoop() noexcept;
    void switchToFiber(FiberObject* fiber) noexcept;
    void dispatch(FiberObject* front_) noexcept;

private:
    static const size_t SCHEDULER_STACK_SIZE = 512 * 1024;

private:
    std::deque<FiberObject*> runQueue;
    FiberObject* current;
    FiberObject* mainFiber;
    // scheduler stack to store callee-saved registers and return addresses
    char* stackPtr;
    char* stackTop;
    ExecutionContext context;
};

void schedulerRunLoopTrampoline() noexcept;

extern Scheduler scheduler;

#endif

