#ifndef TABLE_H
#define TABLE_H
#pragma once

#include "philosopher.hpp"
#include "fork.hpp"

class Philosopher;

class Table
{
public:
    explicit Table(size_t seats);
    Fork& leftForkRef(size_t seat);
    Fork& rightForkRef(size_t seat);
    Philosopher& getPhilosopher(size_t seat);

private:
    size_t seats;
    std::vector<Fork> forks;
    std::vector<Philosopher> philosophers;
};

#endif // TABLE_H
