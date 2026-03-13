#include "fiber.hpp"

Fiber::Fiber() noexcept 
// У пустого файбера:
// - нет номера
// - нельзя вызвать join и detach
// - его нет в очереди планировщика
        : routine(nullptr)
        , state(Fiber::State::NotLaunched)
        , id(0)
        , stackPtr(nullptr)
        , context()
        , detachCalled(false)
        , routineCompleted(false)
{
    setupTrampoline();
    scheduler.pushFiber(this);
}

Fiber::Fiber(void(*f)()) noexcept
        : routine(f)
        , state(Fiber::State::NotLaunched)
        , id(getNextFiberId())
        // stack grows in direction of address decrease
        , stackPtr(new char[STACK_SIZE])
        , context(stackPtr + STACK_SIZE - 
            (ExecutionContext::CALLEE_SAVED_REGISTERS_STACK + 1) * 8)
        , detachCalled(false)
        , routineCompleted(false)
{
    std::cout << "Fiber()" << std::endl;
    setupTrampoline();
    scheduler.pushFiber(this);
    std::cout << "Fiber " << id << " stack starts at " << stackPtr << ", ends at " <<
        static_cast<void*>(static_cast<char*>(stackPtr) - STACK_SIZE) << std::endl;
}

Fiber::~Fiber()
{
    std::cout << "Fiber " << id << " dtor call" << std::endl;
    while (!routineCompleted)
    {
        ThisFiber::yield();
    }
    if (joinable())
    {
        std::terminate();
    }
    delete[] stackPtr;
    std::cout << "Fiber " << id << " dtor call ended" << std::endl;
}

Fiber::Fiber(Fiber&& other)
        : routine(other.routine)
        , id(getNextFiberId())
        , detachCalled(other.detachCalled)
{
    other.routine = nullptr;
    other.detachCalled = false;
}

Fiber& Fiber::operator=(Fiber&& other) // noexcept?
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

size_t Fiber::getNextFiberId() const noexcept
{
    ++maxFiberId;
    return maxFiberId;
}

void Fiber::setupTrampoline() noexcept
{
    void(**returnAddressPtr)() = reinterpret_cast<void(**)()>(stackPtr + 
        STACK_SIZE - 8);
    *returnAddressPtr = trampoline;
}

void Fiber::switchToScheduler() noexcept
{
    switchContextAsm(&context, &scheduler.context); 
}

size_t Fiber::maxFiberId = 0;

bool Fiber::joinable() const noexcept
{
    return !detachCalled && routine;
}

size_t Fiber::getId() const noexcept
{
    return id;
}

void Fiber::detach() // const?
{
    if (!joinable())
    {
        throw FiberError();
    }
    detachCalled = true;
}

void Fiber::join() // const?
{
    detach();
    ThisFiber::yield(); // sure?
}

std::strong_ordering Fiber::operator<=>(const Fiber& other) const noexcept
{
    return id <=> other.id;
    // А если подумать, тут weak_ordering, потому что равных файберов на самом
    // деле нет. Или забить?
}

[[noreturn]] void trampoline() noexcept
{
    try
    {
        scheduler.current->routine();
    }
    catch (...)
    {
        std::cout << "Terminate" << std::endl; // TODO: write sth cleverer
        std::terminate();
    }
    scheduler.current->routineCompleted = true;
    scheduler.terminateCurrentFiber();
    assert(false);
}

size_t getId() noexcept
{
    // TODO: implement
    return 0;
}

void yield() noexcept
{
    // TODO: implement
}

Fiber::FiberError::~FiberError() {}

const char* Fiber::FiberError::what() const noexcept
{
    return "Fiber is not joinable";
}

void ThisFiber::yield() noexcept
{
    scheduler.current->state = Fiber::State::Yielded;
    scheduler.current->switchToScheduler();
}
