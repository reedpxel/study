#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H
#pragma once

#include <vector>
#include <cassert>
#include <chrono>
#include <unistd.h>
#include "fork.hpp"
#include "table.hpp"

class Table;

using namespace std::chrono;

class Philosopher
{
public:
    Philosopher(Table& table, size_t seat);
    void eat();
    void think();
    size_t mealsTaken();

private:
    void acquireForks();
    void releaseForks();
    void eatWithForks();

private:
    Table& table;
    size_t seat;
    Fork& leftFork;
    Fork& rightFork;
    size_t meals;
};

#endif // PHILOSOPHER_H
