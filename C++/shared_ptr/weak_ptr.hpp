#include "shared_ptr.hpp" 

#ifndef WEAK_PTR_H
#define WEAK_PTR_H

template <typename T>
class weak_ptr : public SmartPointerBase<T> // TO DO: remove public
{
public:
    using SmartPointerBase<T>::pObject;
    using SmartPointerBase<T>::pCtrlBlock;
    using BaseControlBlock = SmartPointerBase<T>::BaseControlBlock;
    using ControlBlockWithObject = SmartPointerBase<T>::
        ControlBlockWithObject;

    weak_ptr() noexcept 
            : SmartPointerBase<T>{nullptr, nullptr}
    {}

    weak_ptr(const shared_ptr<T>& shptr) noexcept
            : SmartPointerBase<T>{shptr.pObject, shptr.pCtrlBlock}
    {
        if (isNullptr(pObject) && isNullptr(pCtrlBlock)) return;
        incrementWeakCount();
    }

    weak_ptr(const weak_ptr& other) noexcept
            : SmartPointerBase<T>{other.pObject, other.pCtrlBlock}
    {
        if (isNullptr(pObject) && isNullptr(pCtrlBlock)) return;
        incrementWeakCount();
    }

    weak_ptr(weak_ptr&& other) noexcept
            : SmartPointerBase<T>{other.pObject, other.pCtrlBlock}
    {
        other.pObject = nullptr;
        other.pCtrlBlock = nullptr;
    }

    ~weak_ptr() { dtorHelper(); }

    weak_ptr& operator=(const weak_ptr& other) noexcept
    {
        return operatorEqualHelper(other);
    }

    weak_ptr& operator=(const shared_ptr<T>& other) noexcept
    {
        return operatorEqualHelper(other);
    }

    weak_ptr& operator=(weak_ptr&& other) noexcept
    {
        if (this != &other && pObject != other.pObject)
        {
            dtorHelper();
            pObject = other.pObject;
            pCtrlBlock = other.pCtrlBlock;
            other.pObject = nullptr;
            other.pCtrlBlock = nullptr;
        }
        return *this;
    }

    void reset() noexcept
    {
        dtorHelper();
        pCtrlBlock = nullptr;
        pObject = nullptr;
    }

    void swap(weak_ptr& other) noexcept
    {
        std::swap(pObject, other.pObject);
        std::swap(pCtrlBlock, other.pCtrlBlock);
    }

    size_t use_count() const noexcept
    // amount of shared_ptrs that point to the owned object
    {         
        return SmartPointerBase<T>::getSharedCount(); 
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
public: // TO DO: make private
    void incrementWeakCount() noexcept
    {
        if (isNullptr(pCtrlBlock))
        {
            ++(SmartPointerBase<T>::getCtrlBlockWithObject()->weak_count);
        } else {
            ++pCtrlBlock->weak_count;
        }
    }

    void decrementWeakCount() noexcept
    {
        if (isNullptr(pCtrlBlock))
        {
            --(SmartPointerBase<T>::getCtrlBlockWithObject()->weak_count);
        } else {
            --pCtrlBlock->weak_count;
        }
    }

    size_t getWeakCount() const noexcept
    {
        return isNullptr(pCtrlBlock) ? 
            SmartPointerBase<T>::getCtrlBlockWithObject()->weak_count :
            pCtrlBlock->weak_count;
    }

    void dtorHelper() noexcept
    {
        if (isNullptr(pObject))
        {
            if (!isNullptr(pCtrlBlock))
            {
                if (pCtrlBlock->weak_count == 1)
                {
                    delete pCtrlBlock;
                } else {
                    --pCtrlBlock->weak_count;
                }
            }
        } else {
            if (isNullptr(pCtrlBlock))
            {
                ControlBlockWithObject* ctrlBlockWithObjectPtr = 
                    SmartPointerBase<T>::getCtrlBlockWithObject();
                if (!ctrlBlockWithObjectPtr->shared_count &&
                    ctrlBlockWithObjectPtr->weak_count == 1)
                {
                    delete ctrlBlockWithObjectPtr;
                } else {
                    --ctrlBlockWithObjectPtr->weak_count;
                }
            } else {
                if (!pCtrlBlock->shared_count &&
                    pCtrlBlock->weak_count == 1)
                {
                    delete pCtrlBlock;
                } else {
                    --pCtrlBlock->weak_count;
                }
            }
        }
    }

    template <typename U> // to pass shared_ptr and weak_ptr as an argument
    weak_ptr& operatorEqualHelper(const U& other) noexcept
    {
        bool condition = (pObject != other.pObject);
        if constexpr (std::is_same_v<weak_ptr<T>, U>)
        { 
            // *this cannot be passed as an argument, if U is shared_ptr, but 
            // can be passed if U is weak_ptr
            condition = condition && (this != &other);
        }
        if (condition)
        {
            dtorHelper();
            pObject = other.pObject;
            pCtrlBlock = other.pCtrlBlock;
            incrementWeakCount();
        }
        return *this;
    }
};

#endif // WEAK_PTR_H

