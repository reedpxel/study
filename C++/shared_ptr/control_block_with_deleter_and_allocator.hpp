#include "base_control_block.hpp"

#ifndef CONTROL_BLOCK_WITH_DELETER_AND_ALLOCATOR
#define CONTROL_BLOCK_WITH_DELETER_AND_ALLOCATOR

template <typename T, typename Deleter, typename Alloc>
struct ControlBlockWithDeleterAndAllocator : BaseControlBlock
{
    T* ptr;
    [[no_unique_address]] Deleter deleter;
    [[no_unique_address]] Alloc alloc;

    ControlBlockWithDeleterAndAllocator(T* ptr,
                                        Deleter deleter,
                                        Alloc alloc)
            : BaseControlBlock()
            , ptr(ptr)
            , deleter(deleter)
            , alloc(alloc)
    {}

    void destroyObject() noexcept override { deleter(ptr); }

    void destroyThis() noexcept override
    {
        using CtrlBlockAlloc = std::allocator_traits<Alloc>::template 
            rebind_alloc<ControlBlockWithDeleterAndAllocator>;
        CtrlBlockAlloc ctrlBlockAlloc(alloc);
        this->~ControlBlockWithDeleterAndAllocator();
        std::allocator_traits<CtrlBlockAlloc>::deallocate(ctrlBlockAlloc, this,
            1);
    }
};

#endif

