#ifndef SCHEDULER_H
#define SCHEDULER_H
#pragma once

#include "fiber.hpp"
#include "execution_context.hpp"

#include <deque>
#include <memory>
#include <unordered_map> // TODO: remove

int main();
class Fiber;

class Scheduler
{
public:
    Scheduler();
    ~Scheduler();
    void pushFiber(Fiber* fiber);
    void terminateCurrentFiber() noexcept;
    void printSchedulerQueue(); // TODO: remove

public: // TODO: make private
    void runLoop() noexcept;
    void switchToFiber(Fiber* fiber) noexcept;
    void dispatch(Fiber* front_) noexcept;
public: // TODO: make private
    static const size_t SCHEDULER_STACK_SIZE = 1024 * 1024;
public: // TODO: make private
    std::deque<Fiber*> runQueue; // stores pointers, because fiber objects are
                                 // created on stacks of other fibers 
    Fiber* current;
    // scheduler stack to store callee-saved registers and return address
    char* stackPtr;
    char* stackTop;
    ExecutionContext context;
};

void schedulerRunLoopTrampoline();

extern Scheduler scheduler;

#endif

