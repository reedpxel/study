#include "fiber.hpp"

FiberObject::FiberObject(void(*f)()) noexcept
        : routine(f)
        , id(getNextFiberId())
        // stack grows in direction of address decrease
        , stackPtr(id == 1 ? nullptr : new char[STACK_SIZE])
        , context(id == 1 ? nullptr : stackPtr + STACK_SIZE - 
            (ExecutionContext::CALLEE_SAVED_REGISTERS_STACK + 1) * 8)
        , state(FiberObject::State::NotLaunched)
        , detachCalled(false)
        , routineCompleted(false)
{
    setupTrampoline();
    scheduler.pushFiber(this);
}

FiberObject::FiberObject(FiberObject&& other)
        : routine(other.routine)
        , id(getNextFiberId())
        , detachCalled(other.detachCalled)
{
    other.routine = nullptr;
    other.detachCalled = false;
}

FiberObject& FiberObject::operator=(FiberObject&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    routine = other.routine;
    other.routine = nullptr;
    detachCalled = other.detachCalled;
    other.detachCalled = false;
    return *this;
}

FiberObject::~FiberObject()
{
    delete[] stackPtr;
}

size_t FiberObject::getId() const noexcept
{
    return id;
}

FiberObject::State FiberObject::getState() const noexcept
{
    return state;
}

void FiberObject::setState(FiberObject::State state_) noexcept
{
    state = state_;
}

size_t FiberObject::getNextFiberId() const noexcept
{
    ++maxFiberId;
    return maxFiberId;
}

void FiberObject::setupTrampoline() noexcept
{
    if (id == 1)
    {
        return;
    }
    void(**returnAddressPtr)() = reinterpret_cast<void(**)()>(stackPtr + 
        STACK_SIZE - 8);
    *returnAddressPtr = trampoline;
}

void FiberObject::switchToScheduler() noexcept
{
    switchContextAsm(&context, &scheduler.context); 
}

size_t FiberObject::maxFiberId = 0;

[[noreturn]] void trampoline() noexcept
{
    // TODO: exceptions break a fiber
    try
    {
        scheduler.getCurrentFiber()->routine();
    }
    catch (...)
    {
        std::cout << "Uncaught exception went beyond the fiber stack" << 
            std::endl;
        std::terminate();
    }
    scheduler.getCurrentFiber()->routineCompleted = true;
    scheduler.terminateCurrentFiber();
    assert(false);
}

size_t ThisFiber::getId() noexcept
{
    return scheduler.getCurrentFiber()->getId();
}

void ThisFiber::yield() noexcept
{
    scheduler.getCurrentFiber()->setState(FiberObject::State::Yielded);
    scheduler.getCurrentFiber()->switchToScheduler();
}

Fiber::Fiber(void(*routine)())
        : fiber(new FiberObject(routine))
{}

Fiber::Fiber(Fiber&& other) noexcept
        : fiber(other.fiber)
{
    other.fiber = nullptr;
}

Fiber& Fiber::operator=(Fiber&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    fiber = other.fiber;
    other.fiber = nullptr;
    return *this;
}

Fiber::~Fiber()
{
    if (fiber && !fiber->detachCalled)
    {
        std::terminate();
    }
}

void Fiber::swap(Fiber& other) noexcept
{
    FiberObject* tmp = fiber;
    fiber = other.fiber;
    other.fiber = tmp;
}

bool Fiber::joinable() const noexcept
{
    if (fiber)
    {
        return !fiber->detachCalled;
    }
    return false;
}

size_t Fiber::getId() const noexcept
{
    if (!fiber)
    {
        return 0;
    }
    return fiber->getId();
}

void Fiber::detach()
{
    if (!joinable())
    {
        throw FiberError{};
    }
    fiber->detachCalled = true;
}

void Fiber::join()
{
    detach();
    ThisFiber::yield();
}

std::strong_ordering Fiber::operator<=>(const Fiber& other) const noexcept
{
    if (!fiber || !other.fiber)
    {
        return fiber <=> other.fiber;
    }
    return fiber->id <=> other.fiber->id;
}

Fiber::FiberError::~FiberError() {}

const char* Fiber::FiberError::what() const noexcept
{
    return "Fiber is not joinable";
}

void swap(Fiber& l, Fiber& r) noexcept
{
    l.swap(r);
}

bool hasReadyFibers() noexcept
{
    return !scheduler.runQueue.empty();
}

