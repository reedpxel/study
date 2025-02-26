#include "shared_ptr.hpp" 

#ifndef WEAK_PTR_H
#define WEAK_PTR_H

template <typename T>
class weak_ptr
{
public:
    constexpr weak_ptr() noexcept = default;

    weak_ptr(const weak_ptr& other) noexcept
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        if (!isNullptr(pCtrlBlock)) ++pCtrlBlock->weakCount;
    }

    template <typename Y>
    weak_ptr(const weak_ptr<Y>& other) noexcept
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        if (!isNullptr(pCtrlBlock)) ++pCtrlBlock->weakCount;
    }

    template <typename Y>
    weak_ptr(const shared_ptr<Y>& other) noexcept
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        if (!isNullptr(pCtrlBlock)) ++pCtrlBlock->weakCount;
    }
    
    weak_ptr(weak_ptr&& other)
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        other.pObject = nullptr;
        other.pCtrlBlock = nullptr;
    }

    template <typename Y>
    weak_ptr(weak_ptr<Y>&& other)
            : pObject(other.pObject)
            , pCtrlBlock(other.pCtrlBlock)
    {
        other.pObject = nullptr;
        other.pCtrlBlock = nullptr;
    }

    ~weak_ptr() { decrementWeakCount(); }

    template <typename Y>
    weak_ptr& operator=(const weak_ptr<Y>& other) noexcept
    {
        if (this != &other && pObject != other.pObject)
        {
            decrementWeakCount();
            pObject = other.pObject;
            pCtrlBlock = other.pCtrlBlock;
            if (!isNullptr(pCtrlBlock)) ++pCtrlBlock->weakCount;
        }
        return *this;
    }

    weak_ptr& operator=(const weak_ptr& other) noexcept
    {
        return operator=<T>(other);
    }
    
    template <typename Y>
    weak_ptr& operator=(weak_ptr<Y>&& other) noexcept
    {
        if (this != &other && pObject != other.pObject)
        {
            decrementWeakCount();
            pObject = other.pObject;
            pCtrlBlock = other.pCtrlBlock;
            other.pObject = nullptr;
            other.pCtrlBlock = nullptr;
        }
        return *this;
    }

    weak_ptr& operator=(weak_ptr&& other) noexcept
    {
        return operator=<T>(std::move(other));
    }

    template <typename Y>
    weak_ptr& operator=(const shared_ptr<Y>& other) noexcept
    {
        if (pObject != other.pObject)
        {
            decrementWeakCount();
            pObject = other.pObject;
            pCtrlBlock = other.pCtrlBlock;
            if (!isNullptr(pCtrlBlock)) ++pCtrlBlock->weakCount;
        }
        return *this;       
    }

    void reset() noexcept
    {
        decrementWeakCount();
        pCtrlBlock = nullptr;
        pObject = nullptr;
    }

    void swap(weak_ptr& other) noexcept
    {
        if (this == &other) return;
        std::swap(pObject, other.pObject);
        std::swap(pCtrlBlock, other.pCtrlBlock);
    }

    size_t use_count() const noexcept
    // amount of shared_ptrs that point to the owned object
    {         
        return isNullptr(pCtrlBlock) ? 0 : pCtrlBlock->sharedCount;
    }
 
    bool expired() const noexcept // check whether object is deleted
    {
        return !use_count();
    }

    shared_ptr<T> lock() const noexcept 
    // creates a shared_ptr that manages the object.
    {
        return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
    }

    template <typename U>
    bool owner_before(const weak_ptr<U>& other) const noexcept
    {
        return pObject == static_cast<T*>(other.pObject);
    }
 
    template <typename U>
    bool owner_before(const shared_ptr<U>& other) const noexcept
    {
        return pObject == static_cast<T*>(other.pObject);
    }
private:
    void decrementWeakCount() noexcept
    {
        if (isNullptr(pCtrlBlock)) return;
        if (pCtrlBlock->weakCount == 1 && !pCtrlBlock->sharedCount)
        {
            pCtrlBlock->destroyThis();
        } else {
            --pCtrlBlock->weakCount;
        }
    }
private:
    T* pObject = nullptr;
    BaseControlBlock* pCtrlBlock = nullptr;
};

#endif // WEAK_PTR_H

