#include "atomic.h"

Atomic::Atomic(unsigned long long value_ = 0) : buffer_(value_) {}

value_t Atomic::load() { return asm_load(&buffer_); }

void Atomic::store(value_t value_) { asm_store(&buffer_, value_); }

value_t Atomic::exchange(value_t value_)
{
    return asm_exchange(&buffer_, value_); 
}

value_t Atomic::fetch_add(value_t arg) { return asm_fetch_add(&buffer_, arg); }

value_t Atomic::fetch_sub(value_t arg) { return asm_fetch_sub(&buffer_, arg); }

value_t Atomic::fetch_and(value_t arg) { return asm_fetch_and(&buffer_, arg); }

value_t Atomic::fetch_or(value_t arg) { return asm_fetch_or(&buffer_, arg); }

value_t Atomic::fetch_xor(value_t arg) { return asm_fetch_xor(&buffer_, arg); }

value_t Atomic::operator+=(value_t arg) { return fetch_add(arg); }

value_t Atomic::operator-=(value_t arg) { return fetch_sub(arg); }

value_t Atomic::operator&=(value_t arg) { return fetch_and(arg); }

value_t Atomic::operator|=(value_t arg) { return fetch_or(arg); }

value_t Atomic::operator^=(value_t arg) { return fetch_xor(arg); }

value_t Atomic::operator++()
{
    value_t tmp = buffer_;
    fetch_add(1);
    return tmp;
}

value_t Atomic::operator++(int) { return fetch_add(1); }

value_t Atomic::operator--()
{
    value_t tmp = buffer_;
    fetch_sub(1);
    return tmp;
}

value_t Atomic::operator--(int) { return fetch_sub(1); }
