#ifndef ENABLE_SHARED_FROM_THIS_H
#define ENABLE_SHARED_FROM_THIS_H

#include "weak_ptr.hpp"

template <typename T>
struct enable_shared_from_this
{
    weak_ptr<T> wptr;

    enable_shared_from_this() {}

    shared_ptr<T> shared_from_this() const noexcept 
    {
        return shared_ptr<T>(wptr); 
    }

    friend class shared_ptr<T>;
};

#endif
