#include "philosopher.hpp"

Philosopher::Philosopher(Table& table, size_t seat)
        : table(table)
        , seat(seat)
        , leftFork(table.leftForkRef(seat))
        , rightFork(table.rightForkRef(seat))
        , meals(0)
{}

void Philosopher::eat()
{
    acquireForks();
    eatWithForks();
    releaseForks();
}

void Philosopher::think()
{
    usleep(100);
}

size_t Philosopher::mealsTaken()
{
    return meals;
}

void Philosopher::acquireForks()
{
// To avoid deadlock, at least one thread must take locks in another order in
// comparison with another, this way wait-for graph will never have a cycle
    Fork& firstForkToTake = seat == 0 ? leftFork : rightFork;
    Fork& secondForkToTake = seat == 0 ? rightFork : leftFork;
//    Fork& firstForkToTake = seat % 2 == 0 ? leftFork : rightFork;
//    Fork& secondForkToTake = seat % 2 == 0 ? rightFork : leftFork;
    firstForkToTake.lock();
    secondForkToTake.lock();
}

void Philosopher::releaseForks()
{
    leftFork.unlock();
    rightFork.unlock();
}

void Philosopher::eatWithForks()
{
    assert(leftFork.lockedByThisThread() && rightFork.lockedByThisThread());
    usleep(100);
    ++meals;
}

