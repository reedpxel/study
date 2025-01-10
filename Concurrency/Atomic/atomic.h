#ifndef ATOMIC
#define ATOMIC
#include "c_functions.h"
#define value_t unsigned long long

class Atomic
{
    value_t buffer_;
public:
    Atomic(value_t value_);

    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;
    Atomic(Atomic&&) = delete;
    Atomic& operator=(Atomic&&) = delete;

    value_t load(); // returns a value that the atomic stores
    void store(value_t value_); // changes the data that atomic stores to value_
    value_t exchange(value_t value_); // replaces the data that atomic stores 
    // to value_, returns the old data
    value_t fetch_add(value_t arg);
    value_t fetch_sub(value_t arg);
    value_t fetch_and(value_t arg);
    value_t fetch_or(value_t arg);
    value_t fetch_xor(value_t arg);
    value_t operator+=(value_t arg);
    value_t operator-=(value_t arg);
    value_t operator&=(value_t arg);
    value_t operator|=(value_t arg);
    value_t operator^=(value_t arg);
    value_t operator++();
    value_t operator++(int);
    value_t operator--();
    value_t operator--(int);
};

#endif 