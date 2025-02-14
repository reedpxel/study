#include <iostream> // TO DO: remove
#include <memory>

template <typename T>
class shared_ptr
{
public: // TO DO: remove
    T* ptr;
    size_t* count;
    // deleter ?
    // allocator ?
    
    struct ControlBlock
    {
        T value;
        size_t count;
    };

    shared_ptr(ControlBlock* cb);
public:
    shared_ptr() noexcept
            : ptr(nullptr)
            , count(nullptr)
    {}

    shared_ptr(std::nullptr_t)
            : shared_ptr() 
    {}

    shared_ptr(T* ptr)
            : ptr(ptr)
            , count(new size_t(1))
    {}

    shared_ptr(const shared_ptr& other) noexcept
            : ptr(other.ptr)
            , count(other.count)
    {
        if (count) ++*count;
    }

    shared_ptr(shared_ptr&& other) noexcept
            : ptr(other.ptr)
            , count(other.count)
    {
        other.ptr = nullptr;
        other.count = nullptr;
    }

    // TO DO: check other ctors

    shared_ptr& operator=(const shared_ptr& other)
    {
        if (this != &other && ptr != other.ptr)
        {
            if (count)
            {
                if (*count == 1)
                {
                    delete ptr;
                    delete count;
                } else {
                    --*count;
                }
            }
            ptr = other.ptr;
            count = other.count;
            if (count) ++*count;
        }
        return *this;
    }

    shared_ptr& operator=(shared_ptr&& other)
    {
        if (this != &other && ptr != other.ptr)
        {
            if (count)
            {
                if (*count == 1)
                {
                    delete ptr;
                    delete count;
                } else {
                    --*count;
                }
            }
            ptr = other.ptr;
            count = other.count;
            other.ptr = nullptr;
            other.count = nullptr;
        }
        return *this;
    }

    // unlike in unique_ptr, in shared_ptr deleter is called even if ptr is
    // nullptr
    ~shared_ptr()
    {
        if (!count)
        {
            delete ptr;
            return;
        }
        if (*count == 1)
        {
            delete ptr;
            delete count;
            return;
        }
        --*count;
    }

    void reset() noexcept
    {
        if (!count) return;
        if (*count == 1)
        {
            delete ptr;
            delete count;
            ptr = nullptr;
            count = nullptr;
            return;
        }
        --*count;
        ptr = nullptr;
        count = nullptr;
    }

    void reset(T* newPtr) // TO DO: add deleter and allocator
    {
        if (!count)
        {
            count = new size_t(1);
            ptr = newPtr;
            return;
        }
        if (*count == 1)
        {
            size_t* countPtr = new size_t(1);
            delete ptr;
            delete count;
            ptr = newPtr;
            count = countPtr;
            return;
        }
        size_t* countPtr = new size_t(1);
        --*count;
        ptr = newPtr;
        count = countPtr;
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
    size_t use_count() const noexcept { return count ? *count : 0; }
    explicit operator bool() const noexcept { return ptr; }

    template <typename U> // TO DO: add weak_ptr version
    bool owner_before(const shared_ptr& other) const noexcept
    {
        return ptr == static_cast<T*>(other.ptr);
    }

    template <typename U, typename... Args>
    friend shared_ptr<U> make_shared(Args&&... args);
};

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args)
{
    auto* p = new shared_ptr<T>::ControlBlock{T(std::forward<Args>(args)...),
        1};
    return shared_ptr<T>(p);
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

