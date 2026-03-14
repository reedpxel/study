#include "scheduler.hpp"

Scheduler::Scheduler()
        : runQueue{}
        , current(nullptr)
        , stackPtr(new char[SCHEDULER_STACK_SIZE])
        , stackTop(stackPtr + SCHEDULER_STACK_SIZE - 8)
        , context(stackTop - 
            ExecutionContext::CALLEE_SAVED_REGISTERS_STACK * 8)
{
    void(**trampolinePtr)() = reinterpret_cast<void(**)()>(stackPtr + 
        SCHEDULER_STACK_SIZE - 8);
    *trampolinePtr = schedulerRunLoopTrampoline;
}

Scheduler::~Scheduler()
{
    while (runQueue.size() != 1)
    {
        ThisFiber::yield();
    }
    delete[] stackPtr;
}

void Scheduler::pushFiber(FiberObject* fiber)
{
    if (runQueue.empty())
    {
        // fiber is main() fiber
        current = fiber;
        fiber->detachCalled = true;
        fiber->routineCompleted = true;
    }
    runQueue.push_back(fiber);
}

void Scheduler::terminateCurrentFiber() noexcept
{
    // В очереди планировщика всегда есть минимум один файбер - файбер main().
    // Этот метод никогда не вызовется от файбера main(), т.к. этот файбер
    // запускается не через trampoline, сразу runnable.
    // Cледовательно, он вызывается только от файберов, не являющихся main().
    // Следовательно, не нужно обрабатывать ситуацию, когда очередь
    // планировщика пуста.
    // Если этот метод вызвался, размер очереди минимум 2.
    scheduler.current->setState(FiberObject::State::Terminated);
    scheduler.current->switchToScheduler();
}

FiberObject* Scheduler::getCurrentFiber() const noexcept
{
    return current;
}

void Scheduler::runLoop() noexcept
{
    while (!runQueue.empty())
    {
        FiberObject* front_ = runQueue.front();
        dispatch(front_);
    }
}

void Scheduler::dispatch(FiberObject* front_) noexcept
{
    switch (front_->getState())
    {
        case FiberObject::State::NotLaunched:
            current = front_;
            switchToFiber(front_); // calls current->trampoline
            break;
        case FiberObject::State::Runnable: 
            // the fiber was launched, called yield, was pushed in the end 
            // of the run queue and now is in front of the run queue
            current = front_;
            switchToFiber(front_);
            break;
        case FiberObject::State::Yielded:
            current = nullptr;
            front_->setState(FiberObject::State::Runnable);
            runQueue.pop_front();
            runQueue.push_back(front_);
            break;
        case FiberObject::State::Terminated:
            current = nullptr;
            runQueue.pop_front();
            delete front_;
            break;
        default:
            std::cout << "Erroneous fiber state" << std::endl;
            std::terminate();
    }
}

void Scheduler::switchToFiber(FiberObject* fiber) noexcept
{
    switchContextAsm(&context, &fiber->context);
}

void schedulerRunLoopTrampoline()
{
    scheduler.runLoop();
}

Scheduler scheduler;
FiberObject mainFiber(reinterpret_cast<void(*)()>(&main));

