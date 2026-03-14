#ifndef SCHEDULER_H
#define SCHEDULER_H
#pragma once

#include "fiber.hpp"
#include "execution_context.hpp"

#include <deque>
#include <memory>

int main();
class FiberObject;

class Scheduler
{
public:
    Scheduler();
    ~Scheduler();
    void pushFiber(FiberObject* fiber);
    void terminateCurrentFiber() noexcept;
    FiberObject* getCurrentFiber() const noexcept;

public: // TODO: make private
    void runLoop() noexcept;
    void switchToFiber(FiberObject* fiber) noexcept;
    void dispatch(FiberObject* front_) noexcept;
public: // TODO: make private
    static const size_t SCHEDULER_STACK_SIZE = 512 * 1024;
public: // TODO: make private
    std::deque<FiberObject*> runQueue;
    FiberObject* current;
    // scheduler stack to store callee-saved registers and return address
    char* stackPtr;
    char* stackTop;
    ExecutionContext context;
};

void schedulerRunLoopTrampoline();

extern Scheduler scheduler;

#endif

