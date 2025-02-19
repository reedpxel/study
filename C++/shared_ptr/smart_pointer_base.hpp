#ifndef SMART_POINTER_BASE_H
#define SMART_POINTER_BASE_H

template <typename T>
bool isNullptr(T* ptr);

template <typename T>
struct SmartPointerBase
{
    struct BaseControlBlock
    {
        size_t shared_count;
        size_t weak_count;
    };

    struct ControlBlockWithObject : BaseControlBlock
    {
        T value;
    };

    T* pObject;
    BaseControlBlock* pCtrlBlock;
public: // TO DO: set private
    size_t getSharedCount() const noexcept 
    {
        if (isNullptr(pCtrlBlock))
        {
            if (isNullptr(pObject)) return 0;
            ControlBlockWithObject* pWithObject = reinterpret_cast<
                ControlBlockWithObject*>(reinterpret_cast<size_t*>(pObject) 
                - 2);
            return pWithObject->shared_count;
        }
        return pCtrlBlock->shared_count;
    }

    size_t getSharedCountNoCtrlBlockNoCheck() const noexcept
    {
        return *(reinterpret_cast<size_t*>(pObject) - 2);
    }

    size_t getWeakCountNoCtrlBlockNoCheck() const noexcept
    {
        return *(reinterpret_cast<size_t*>(pObject) - 1);
    }

    ControlBlockWithObject* getCtrlBlockWithObject() const noexcept
    {
        return reinterpret_cast<ControlBlockWithObject*>(reinterpret_cast<
            size_t*>(pObject) - 2);
    }

    void incrementSharedCount() noexcept
    {
        if (isNullptr(pCtrlBlock)) ++(getCtrlBlockWithObject()->shared_count);
        else ++pCtrlBlock->shared_count;
    }
};

#endif

