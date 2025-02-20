#include <iostream> // TO DO: remove
#include "smart_pointer_base.hpp"

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
class shared_ptr : public SmartPointerBase<T> // TO DO: set private
{
public: // TO DO: make protected
    using SmartPointerBase<T>::pObject;
    using SmartPointerBase<T>::pCtrlBlock;
    using BaseControlBlock = SmartPointerBase<T>::BaseControlBlock;
    using ControlBlockWithObject = SmartPointerBase<T>::
        ControlBlockWithObject;

    void operatorEqualHelper() noexcept
    {
        if (!isNullptr(pObject))
        {
            if (isNullptr(pCtrlBlock))
            {
                if (SmartPointerBase<T>::getSharedCountNoCtrlBlockNoCheck() == 
                    1)
                {
                    pObject->~T();
                    if (!SmartPointerBase<T>::getWeakCountNoCtrlBlockNoCheck())
                    {
                        operator delete(SmartPointerBase<T>::
                            getCtrlBlockWithObject(), sizeof(
                            ControlBlockWithObject));
                        pObject = nullptr;
                    } else {
                        SmartPointerBase<T>::getCtrlBlockWithObject()->
                            shared_count = 0;
                    }
                } else {
                    --(SmartPointerBase<T>::getCtrlBlockWithObject()->
                        shared_count);
                }
            } else {
                if (pCtrlBlock->shared_count == 1)
                {
                    delete pObject;
                    pObject = nullptr;
                    if (!pCtrlBlock->weak_count)
                    {
                        delete pCtrlBlock;
                        pCtrlBlock = nullptr;
                    } else {
                        pCtrlBlock->shared_count = 0;
                    }
                } else {
                    --pCtrlBlock->shared_count;
                }
            } 
        }
    }

    shared_ptr(ControlBlockWithObject* cb)
            : SmartPointerBase<T>{&cb->value, nullptr}
    {}

    shared_ptr(T* pObject, BaseControlBlock* pCtrlBlock)
            : SmartPointerBase<T>{pObject, pCtrlBlock}
    {} // for weak_pObject(const shared_ptr&) 
public:
    shared_ptr() noexcept
            : SmartPointerBase<T>{nullptr, nullptr}
    {}

    shared_ptr(std::nullptr_t) noexcept
            : shared_ptr() 
    {}

    shared_ptr(T* pObject)
            : SmartPointerBase<T>{pObject, new BaseControlBlock{1, 0}}
    {}

    shared_ptr(const shared_ptr& other) noexcept
            : SmartPointerBase<T>{other.pObject, other.pCtrlBlock}
    {
        if (!isNullptr(pObject)) SmartPointerBase<T>::incrementSharedCount();
    }

    explicit shared_ptr(const weak_ptr<T>& other) noexcept
            : SmartPointerBase<T>{nullptr, nullptr}
    {
        if (other.expired()) return;
        pObject = other.pObject;
        pCtrlBlock = other.pCtrlBlock;
        SmartPointerBase<T>::incrementSharedCount();
    }

    shared_ptr(shared_ptr&& other) noexcept
            : SmartPointerBase<T>{other.pObject, other.pCtrlBlock}
    {
        other.pObject = nullptr;
        other.pCtrlBlock = nullptr;
    }

    // unlike in unique_pObject, in shared_ptr deleter is called even if 
    // pObject is nullptr
    ~shared_ptr()
    {
        if (isNullptr(pObject))
        {
            delete pObject;
            return;
        }
        if (isNullptr(pCtrlBlock))
        {
            if (SmartPointerBase<T>::getSharedCountNoCtrlBlockNoCheck() == 1)
            {
                pObject->~T();
                if (!SmartPointerBase<T>::getWeakCountNoCtrlBlockNoCheck())
                {
                    operator delete(SmartPointerBase<T>::
                        getCtrlBlockWithObject(), sizeof(
                        ControlBlockWithObject));
                } else {
                    SmartPointerBase<T>::getCtrlBlockWithObject()->shared_count
                        = 0;
                }
            } else {
                --(SmartPointerBase<T>::getCtrlBlockWithObject()->
                    shared_count);
            }
        } else {
            if (pCtrlBlock->shared_count == 1)
            {
                delete pObject;
                if (!pCtrlBlock->weak_count)
                {
                    delete pCtrlBlock;
                } else {
                    pCtrlBlock->shared_count = 0;
                }
            } else {
                --pCtrlBlock->shared_count;
            }
        }
    }
    // TO DO: check other ctors

    shared_ptr& operator=(const shared_ptr& other) noexcept
    {
        if (this != &other && pObject != other.pObject)
        {
            // 1. --shared_count current control block and delete pObject, if 
            // shared_count == 0
            operatorEqualHelper();
            // 2. copying other's fields
            pObject = other.pObject;
            pCtrlBlock = other.pCBlock;
            // 3. ++shared_count
            if (isNullptr(pCtrlBlock))
            {
                ++(SmartPointerBase<T>::getCtrlBlockWithObject()->
                    shared_count);
            } else {
                ++pCtrlBlock->shared_count;
            }
        }
        return *this;
    }

    shared_ptr& operator=(shared_ptr&& other) noexcept
    {
        if (this != &other && pObject != other.pObject)
        {
            // 1. the same as in copying op=
            operatorEqualHelper();
            // 2. moving fields
            pObject = other.pObject;
            pCtrlBlock = other.pCBlock;
            other.pObject = nullptr;
            other.pCtrlBlock = nullptr;
        }
        return *this;
    }

    void reset() noexcept
    {
        operatorEqualHelper();
        pObject = nullptr;
        pCtrlBlock = nullptr;
    }

    void reset(T* newPtr) // TO DO: add deleter and allocator
    {
        operatorEqualHelper();
        pObject = newPtr;
        pCtrlBlock = new BaseControlBlock{1, 0};
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
        return SmartPointerBase<T>::getSharedCount(); 
    }

    explicit operator bool() const noexcept 
    { 
        return SmartPointerBase<T>::getSharedCount(); 
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
};

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args)
{
    auto pCtrlBlock = new shared_ptr<T>::ControlBlockWithObject{1, 0, 
        std::forward<Args>(args)...};
    return shared_ptr<T>(pCtrlBlock);
    // теперь во всех методах надо рассматривать 2 ситуации:
    // 1. count nullptr, значит, shared_ptr создан через make_shared и к
    // счетчикам надо обращаться как к reinterpret_cast<size_t*>(pObject + 1) -
    // shared_count и reinterpret_cast<size_t*>(pObject + 1) + 1 - weak_count
    // 2. count не nullptr, значит, shared_ptr создан ручным вызовом new в кре,
    // тогда к счетчику нужно обращаться через *count
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const shared_ptr<T>& shptr) 
    noexcept
{
    out << shptr.pObject;
    return out;
}

#endif // SHARED_PTR_H

void* operator new(size_t n)
{ 
    void* ret = malloc(n);
    std::cout << n << " bytes allocated at " << ret << std::endl;
    return ret;
}

void operator delete(void* pObject)
{
    std::cout << "delete at " << pObject << std::endl;
    free(pObject);
}

void operator delete(void* pObject, size_t n) 
{ 
    std::cout << "delete " << n << " bytes at " << pObject << std::endl;
    free(pObject); 
}

