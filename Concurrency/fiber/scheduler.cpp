#include "scheduler.hpp"

Scheduler::Scheduler()
        : runQueue{}
        , current(nullptr)
        , stackPtr(new char[SCHEDULER_STACK_SIZE])
        , stackTop(stackPtr + SCHEDULER_STACK_SIZE - 8)
        , context(stackTop - 
            ExecutionContext::CALLEE_SAVED_REGISTERS_STACK * 8)
{
    void(**trampolinePtr)() = reinterpret_cast<void(**)()>(stackPtr + SCHEDULER_STACK_SIZE - 8);
    *trampolinePtr = schedulerRunLoopTrampoline;
//    void(**arr)() = reinterpret_cast<void(**)()>(schedulerStack);
//    arr[ExecutionContext::CALLEE_SAVED_REGISTERS_STACK] = 
//        schedulerRunLoopTrampoline;
    std::cout << "Scheduler()" << std::endl;
}

Scheduler::~Scheduler()
{
    std::cout << "~Scheduler()" << std::endl;
    delete[] stackPtr;
}

void Scheduler::pushFiber(Fiber* fiber)
{
    if (runQueue.empty())
    {
        // fiber is main() fiber
        current = fiber;
        fiber->state = Fiber::State::Yielded;
        fiber->detachCalled = true;
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
    scheduler.current->state = Fiber::State::Terminated;
    scheduler.current->switchToScheduler();
}

void Scheduler::printSchedulerQueue()
{
    std::unordered_map<int, std::string> stateStr = 
    {
         std::make_pair(0, "NotStarted"),
         std::make_pair(1, "Running"),
         std::make_pair(2, "Runnable"),
         std::make_pair(3, "Terminated"),
    };
    std::cout << "Queue size: " << runQueue.size() << " End ||| ";
    for (auto it = runQueue.end() - 1; ; --it)
    {
        std::cout << (*it)->getId() << ' ' << 
            stateStr[static_cast<int>((*it)->state)] << " | ";
        if (it == runQueue.begin())
        {
            break;
        }
    }
    std::cout << "||| Start" << std::endl;
}

void Scheduler::runLoop() noexcept
{
    while (!runQueue.empty())
    {
        Fiber* front_ = runQueue.front();
        switch (front_->state)
        {
            case Fiber::State::NotLaunched:
                current = front_;
                switchToFiber(front_); // calls current->trampoline
                break;
            case Fiber::State::Runnable: 
                // the fiber was launched, called yield, was pushed in the end 
                // of the run queue and now is in front of the run queue
                current = front_;
                switchToFiber(front_);
                break;
            case Fiber::State::Yielded:
                current = nullptr;
                front_->state = Fiber::State::Runnable;
                runQueue.pop_front();
                runQueue.push_back(front_);
                break;
            case Fiber::State::Terminated:
                current = nullptr;
                runQueue.pop_front();
                break;
            default:
                std::cout << "Erroneous fiber state" << std::endl;
                std::terminate();
        }
    }
}

void Scheduler::switchToFiber(Fiber* fiber) noexcept
{
    switchContextAsm(&context, &fiber->context);
}

void schedulerRunLoopTrampoline()
{
    scheduler.runLoop();
}

Scheduler scheduler;
Fiber mainFiber(reinterpret_cast<void(*)()>(&main));

