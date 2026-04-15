#include "table.hpp"

Table::Table(size_t seats)
        : seats(seats)
        , forks(seats)
        , philosophers{}
{
    for (size_t i = 0; i < seats; ++i)
    {
        philosophers.emplace_back(*this, i);
    }
}

Fork& Table::leftForkRef(size_t seat)
{
    return forks[seat];
}

Fork& Table::rightForkRef(size_t seat)
{
    return forks[(seat + 1) % seats];
}

Philosopher& Table::getPhilosopher(size_t seat)
{
    return philosophers[seat];
}
