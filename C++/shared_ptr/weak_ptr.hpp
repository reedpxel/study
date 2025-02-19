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

    ~weak_ptr()
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
    weak_ptr& operator=(const weak_ptr& other) noexcept;
    weak_ptr& operator=(const shared_ptr<T>& other) noexcept;
    weak_ptr& operator=(weak_ptr&& other) noexcept;
    void reset() noexcept;
    void swap(weak_ptr& other) noexcept;
    size_t use_count() const noexcept; // amount of shared_ptrs that point to
                                       // the owned object
    bool expired() const noexcept; // check whether object is deleted
    shared_ptr<T> lock() const noexcept; // creates a shared_ptr that manages 
                                         // the object. Returns expired ? 
                                         // shared_ptr() : shared_ptr(*this)
    template <typename U>
    bool owner_before(const weak_ptr& other) const noexcept;
 
    template <typename U>
    bool owner_before(const shared_ptr<T>& other) const noexcept;
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
};

#endif // WEAK_PTR_H

