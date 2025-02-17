#include <iostream> // TO DO: remove
#include <memory>

template <typename T>
class shared_ptr
{
public: // TO DO: remove
    struct BaseControlBlock // хранится при создании через кр shared_ptr c
                            // ручным вызовом new
    {
        size_t shared_count;
        size_t weak_count;
    };

    struct ControlBlockWithObject : BaseControlBlock 
    // хранится при создании через make_shared. Обращения к полям shared_count 
    // и weak_count через сдвиги
    {
        T value;
    };

    T* ptr;
    BaseControlBlock* pCBlock;
    // deleter ?
    // allocator ?

//    struct ControlBlock // problem 1 solution
//    {
//        T value;
//        size_t count;
//    };
    template <typename U>
    bool isNullptr(U* ptr) const noexcept
    {
    #ifdef __SANITIZE_ADDRESS__
        return ptr == nullptr || ptr == (U*)0xbebebebebebebebe;
    #else 
        return ptr == nullptr;
    #endif
    }

    size_t getSharedCount() const noexcept 
    {
        return isNullptr(pCBlock) ? 
            isNullptr(ptr) ? 0 : *(reinterpret_cast<size_t*>(ptr) - 2) :
            pCBlock->shared_count;
    }

    size_t getWeakCount() const noexcept
    {
        return isNullptr(pCBlock) ? 
            isNullptr(ptr) ? 0 : *(reinterpret_cast<size_t*>(ptr) - 1) :
            pCBlock->weak_count;
    }

    size_t getSharedCountNoCtrlBlockNoCheck() const noexcept
    {
        return *(reinterpret_cast<size_t*>(ptr) - 2);
    }

    size_t getWeakCountNoCtrlBlockNoCheck() const noexcept
    {
        return *(reinterpret_cast<size_t*>(ptr) - 1);
    }

    ControlBlockWithObject* getCtrlBlockWithObject() const noexcept
    {
        return reinterpret_cast<ControlBlockWithObject*>(reinterpret_cast<
            size_t*>(ptr) - 2);
    }

    void incrementSharedCount() noexcept
    {
        if (isNullptr(pCBlock)) ++(getCtrlBlockWithObject()->shared_count);
        else ++pCBlock->shared_count;
    }

    void decrementSharedCount() noexcept
    {
        if (isNullptr(pCBlock)) --(getCtrlBlockWithObject()->shared_count);
        else --pCBlock->shared_count;
    }

    void operatorEqualHelper() noexcept
    {
        if (!isNullptr(ptr))
        {
            if (isNullptr(pCBlock))
            {
                if (getSharedCountNoCtrlBlockNoCheck() == 1)
                {
                    ptr->~T();
                    if (!getWeakCountNoCtrlBlockNoCheck())
                    {
                        operator delete(getCtrlBlockWithObject(), sizeof(
                            ControlBlockWithObject));
                        ptr = nullptr;
                    } else {
                        getCtrlBlockWithObject()->shared_count = 0;
                    }
                } else {
                    --(getCtrlBlockWithObject()->shared_count);
                }
            } else {
                if (pCBlock->shared_count == 1)
                {
                    delete ptr;
                    ptr = nullptr;
                    if (!pCBlock->weak_count)
                    {
                        delete pCBlock;
                        pCBlock = nullptr;
                    } else {
                        pCBlock->shared_count = 0;
                    }
                } else {
                    --pCBlock->shared_count;
                }
            } 
        }
    }

    shared_ptr(ControlBlockWithObject* cb)
          : ptr(&cb->value)
          , pCBlock(nullptr)
    {}
public:
    shared_ptr() noexcept
            : ptr(nullptr)
            , pCBlock(nullptr)
    {}

    shared_ptr(std::nullptr_t) noexcept
            : shared_ptr() 
    {}

    shared_ptr(T* ptr)
            : ptr(ptr)
            , pCBlock(new BaseControlBlock{1, 0})
    {}

    shared_ptr(const shared_ptr& other) noexcept
            : ptr(other.ptr)
            , pCBlock(other.pCBlock)
    {
        if (!isNullptr(ptr)) incrementSharedCount();
    }

    shared_ptr(shared_ptr&& other) noexcept
            : ptr(other.ptr)
            , pCBlock(other.pCBlock)
    {
        other.ptr = nullptr;
        other.pCBlock = nullptr;
    }

    // unlike in unique_ptr, in shared_ptr deleter is called even if ptr is
    // nullptr
    ~shared_ptr()
    {
        if (isNullptr(ptr))
        {
            delete ptr;
            return;
        }
        if (isNullptr(pCBlock))
        {
            if (getSharedCountNoCtrlBlockNoCheck() == 1)
            {
                ptr->~T();
                if (!getWeakCountNoCtrlBlockNoCheck())
                {
                    operator delete(getCtrlBlockWithObject(), sizeof(
                        ControlBlockWithObject));
                } else {
                    getCtrlBlockWithObject()->shared_count = 0;
                }
            } else {
                --(getCtrlBlockWithObject()->shared_count);
            }
        } else {
            if (pCBlock->shared_count == 1)
            {
                delete ptr;
                if (!pCBlock->weak_count)
                {
                    delete pCBlock;
                } else {
                    pCBlock->shared_count = 0;
                }
            } else {
                --pCBlock->shared_count;
            }
        }
    }
    // TO DO: check other ctors

    shared_ptr& operator=(const shared_ptr& other) noexcept
    {
        if (this != &other && ptr != other.ptr)
        {
            // 1. --shared_count current control block and delete ptr, if 
            // shared_count == 0
            operatorEqualHelper();
            // 2. copying other's fields
            ptr = other.ptr;
            pCBlock = other.pCBlock;
            // 3. ++shared_count
            if (isNullptr(pCBlock))
            {
                ++(getCtrlBlockWithObject()->shared_count);
            } else {
                ++pCBlock->shared_count;
            }
        }
        return *this;
    }

    shared_ptr& operator=(shared_ptr&& other) noexcept
    {
        if (this != &other && ptr != other.ptr)
        {
            // 1. the same as in copying op=
            operatorEqualHelper();
            // 2. moving fields
            ptr = other.ptr;
            pCBlock = other.pCBlock;
            other.ptr = nullptr;
            other.pCBlock = nullptr;
        }
        return *this;
    }

    void reset() noexcept
    {
        operatorEqualHelper();
        ptr = nullptr;
        pCBlock = nullptr;
    }

    void reset(T* newPtr) // TO DO: add deleter and allocator
    {
        operatorEqualHelper();
        ptr = newPtr;
        pCBlock = new BaseControlBlock{1, 0};
    }

    void swap(shared_ptr& other) noexcept
    {
        if (this == &other) return;
        std::swap(ptr, other.ptr);
        std::swap(count, other.count);
    }

    T* get() const noexcept { return ptr; }
    T& operator*() const noexcept { return *ptr; }
    T* operator->() const noexcept { return ptr; }
    size_t use_count() const noexcept { return getSharedCount(); }
    explicit operator bool() const noexcept { return getSharedCount(); }

    template <typename U>
    bool owner_before(const shared_ptr& other) const noexcept
    {
        return ptr == static_cast<T*>(other.ptr);
    }

    template <typename U>
    bool owner_before(const weak_ptr& other) const noexcept;

    template <typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&... args);
};

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args)
{
    auto pCBlock = new shared_ptr<T>::ControlBlockWithObject{1, 0, 
        std::forward<Args>(args)...};
    return shared_ptr<T>(pCBlock);
    // теперь во всех методах надо рассматривать 2 ситуации:
    // 1. count nullptr, значит, shared_ptr создан через make_shared и к
    // счетчикам надо обращаться как к reinterpret_cast<size_t*>(ptr + 1) -
    // shared_count и reinterpret_cast<size_t*>(ptr + 1) + 1 - weak_count
    // 2. count не nullptr, значит, shared_ptr создан ручным вызовом new в кре,
    // тогда к счетчику нужно обращаться через *count
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const shared_ptr<T>& shptr) 
    noexcept
{
    out << *shptr;
    return out;
}

void* operator new(size_t n)
{ 
    void* ret = malloc(n);
    std::cout << n << " bytes allocated at " << ret << std::endl;
    return ret;
}

void operator delete(void* ptr)
{
    std::cout << "delete at " << ptr << std::endl;
    free(ptr);
}

void operator delete(void* ptr, size_t n) 
{ 
    std::cout << "delete " << n << " bytes at " << ptr << std::endl;
    free(ptr); 
}

template <typename T>
struct Debug
{
    Debug() = delete;
};

