#include "base_control_block.hpp"

#ifndef CONTROL_BLOCK_MAKE_SHARED
#define CONTROL_BLOCK_MAKE_SHARED

template <typename T, typename Alloc>
struct ControlBlockMakeShared : BaseControlBlock
{
    T value;
    [[no_unique_address]] Alloc alloc;

    using ControlBlockMakeSharedAlloc = std::allocator_traits<Alloc>::template 
        rebind_alloc<ControlBlockMakeShared>;

    template <typename... ValueArgs>
    ControlBlockMakeShared(Alloc alloc, ValueArgs&&... args)
        : BaseControlBlock()
        , value(std::forward<ValueArgs>(args)...)
        , alloc(alloc)
    {}

    void destroyObject() noexcept override
    {
        using ValueAlloc = std::allocator_traits<Alloc>::template rebind_alloc<
            T>;
        ValueAlloc valueAlloc(alloc);
        std::allocator_traits<ValueAlloc>::destroy(valueAlloc, &value);
    }

    void destroyThis() noexcept override
    {
        using CtrlBlockAlloc = std::allocator_traits<Alloc>::template 
            rebind_alloc<ControlBlockMakeShared>;
        CtrlBlockAlloc ctrlBlockAlloc(alloc);
        std::allocator_traits<CtrlBlockAlloc>::deallocate(ctrlBlockAlloc, 
            this, 1);
        alloc.~Alloc();
    }
};

#endif
