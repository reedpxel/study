#include "base_control_block.hpp"

#ifndef CONTROL_BLOCK_H
#define CONTROL_BLOCK_H

template <typename T>
struct ControlBlock : BaseControlBlock
{
    T* ptr; // а pointer to the allocated object, with correct type and without 
            // a shift
    ControlBlock(T* ptr)
            : BaseControlBlock()
            , ptr(ptr)
    {}

    void destroyObject() noexcept override { delete ptr; }
    void destroyThis() noexcept override { delete this; }
};

#endif

