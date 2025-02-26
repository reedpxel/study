#include "control_block.hpp"
#include "control_block_with_deleter_and_allocator.hpp"
#include "control_block_make_shared.hpp"

#ifndef SHARED_PTR_H
#define SHARED_PTR_H

template <typename U>
bool isNullptr(U* ptr) 
{
#ifdef __SANITIZE_ADDRESS__
    return ptr == nullptr || ptr == (U*)0xbebebebebebebebe;
#else 
    return ptr == nullptr;
#endif
}

template <typename T>
class weak_ptr;

template <typename T>
class shared_ptr
{
private:
    template <typename U, typename Alloc>
    shared_ptr(ControlBlockMakeShared<U, Alloc>* pCtrlBlockMakeShared)
            : pObject(&pCtrlBlockMakeShared->value)
            , pCtrlBlock(pCtrlBlockMakeShared)
    {}
public:
    constexpr shared_ptr() noexcept = default;

    constexpr shared_ptr(std::nullptr_t) noexcept
            : shared_ptr() 
    {}

    template <typename Y>
    explicit shared_ptr(Y* ptr)
            : pObject(ptr)
            , pCtrlBlock(new ControlBlock(pObject))
    {}

    template <typename Y, typename Deleter>
    shared_ptr(Y* ptr, Deleter deleter)
            : pObject(ptr)
            , pCtrlBlock(new ControlBlockWithDeleterAndAllocator(
                ptr, deleter, std::allocator<Y>()))
    {}

    template <typename Y, typename Deleter, typename Alloc>
    shared_ptr(Y* ptr, Deleter deleter, Alloc alloc)
            : pObject(ptr)
            , pCtrlBlock(new ControlBlockWithDeleterAndAllocator(
                ptr, deleter, alloc))
    {} // TO DO: pCtrlBlock must be allocated with custom allocator

    template <typename U>
    shared_ptr(const shared_ptr<U>& other) noexcept
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        if (!isNullptr(pCtrlBlock)) ++pCtrlBlock->sharedCount;
    }
   
    template <typename U>
    shared_ptr(shared_ptr<U>&& other) noexcept
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        other.pObject = nullptr;
        other.pCtrlBlock = nullptr;
    }

    shared_ptr(const shared_ptr& other) noexcept
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        if (!isNullptr(pCtrlBlock)) ++ pCtrlBlock->sharedCount;
    }

    shared_ptr(shared_ptr&& other) noexcept
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        other.pObject = nullptr;
        other.pCtrlBlock = nullptr;
    }

    template <typename Y>
    explicit shared_ptr(const weak_ptr<Y>& other)
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        if (other.expired()) throw std::bad_weak_ptr();
        ++pCtrlBlock->sharedCount;
    }

    // unlike in unique_ptr, in shared_ptr deleter is called even if 
    // pObject is nullptr
    ~shared_ptr() { decrementSharedCount(); }
   
    template <typename U>
    shared_ptr& operator=(const shared_ptr<U>& other) noexcept
    {
        if (this != &other && pObject != other.pObject)
        {
            decrementSharedCount();
            pObject = other.pObject;
            pCtrlBlock = other.pCtrlBlock;
            if (!isNullptr(pCtrlBlock)) ++pCtrlBlock->sharedCount;
        }
        return *this;
    }

    shared_ptr& operator=(const shared_ptr& other) noexcept
    {
        return operator=<T>(other);
    }
    
    template <typename U>
    shared_ptr& operator=(shared_ptr<U>&& other) noexcept
    {
        if (this != &other && pObject != other.pObject)
        {
            decrementSharedCount();
            pObject = other.pObject;
            pCtrlBlock = other.pCtrlBlock;
            other.pObject = nullptr;
            other.pCtrlBlock = nullptr;
        }
        return *this;
    }

    shared_ptr& operator=(shared_ptr&& other) noexcept
    {
        return operator=<T>(std::move(other));
    }

    void reset() noexcept
    {
        decrementSharedCount();
        pObject = nullptr;
        pCtrlBlock = nullptr;
    }

    template <typename U>
    void reset(U* newPtr)
    {
        decrementSharedCount();
        pObject = newPtr;
        pCtrlBlock = new ControlBlock(newPtr);
    }

    void swap(shared_ptr& other) noexcept
    {
        if (this == &other) return;
        std::swap(pObject, other.pObject);
        std::swap(pCtrlBlock, other.pCBlock);
    }

    T* get() const noexcept { return pObject; }
    T& operator*() const noexcept { return *pObject; }
    T* operator->() const noexcept { return pObject; }

    size_t use_count() const noexcept 
    { 
        return isNullptr(pCtrlBlock) ? 0 : pCtrlBlock->sharedCount;
    }

    explicit operator bool() const noexcept 
    { 
        return !isNullptr(pCtrlBlock);
    }

    template <typename U>
    bool owner_before(const shared_ptr<U>& other) const noexcept
    {
        return pObject == static_cast<T*>(other.pObject);
    }

    template <typename U>
    bool owner_before(const weak_ptr<U>& other) const noexcept
    {
        return pObject == static_cast<T*>(other.pObject);
    }

    template <typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&... args);

    template <typename U>
    friend class shared_ptr;

    template <typename U>
    friend class weak_ptr;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& out, const shared_ptr<U>& 
        shptr) noexcept;
private:
    void decrementSharedCount() noexcept
    {
        if (isNullptr(pCtrlBlock)) return;
        if (pCtrlBlock->sharedCount == 1)
        {
            pCtrlBlock->destroyObject();
            if (!pCtrlBlock->weakCount)
            {
                pCtrlBlock->destroyThis();
            } else {
                --pCtrlBlock->sharedCount;
            }
        } else {
            --pCtrlBlock->sharedCount;
        }
    }
private:
    T* pObject = nullptr;
    BaseControlBlock* pCtrlBlock = nullptr;
};

template <typename U, typename... Args>
shared_ptr<U> make_shared(Args&&... args)
{
    using CtrlBlock = ControlBlockMakeShared<U, std::allocator<U>>;
    CtrlBlock* pCtrlBlockMakeShared = new CtrlBlock(std::allocator<U>(), 
        std::forward<Args>(args)...);
    return shared_ptr<U>(pCtrlBlockMakeShared);
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const shared_ptr<T>& shptr) 
    noexcept
{
    out << shptr.pObject;
    return out;
}

#endif // SHARED_PTR_H

