#include <cstddef>
#include <memory>

#ifndef BASE_CONTROL_BLOCK_H
#define BASE_CONTROL_BLOCK_H

struct BaseControlBlock
{
    size_t sharedCount = 1;
    size_t weakCount = 0;

    virtual void destroyObject() = 0;
    virtual void destroyThis() = 0;
    virtual ~BaseControlBlock() {}
};

#endif
