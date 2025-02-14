#include <iostream>
#include <memory>

// Deleter is a functor. With it one can do anything instead of delete call
// (close files, network connections). In STL unique_ptr it is
// std::default_delete<T>
template <typename T>
struct DefaultDelete
{   
    // default operator delete does a check whether ptr == nullptr, no need to
    // do it explicitly
    void operator()(T* ptr) const noexcept { delete ptr; }
};

template <typename T, typename Deleter = DefaultDelete<T>>
class UniquePtr
{
    T* ptr = nullptr;
    [[no_unique_address]] Deleter deleter = Deleter();
public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

    UniquePtr() = default;
    UniquePtr(std::nullptr_t) {}

    explicit UniquePtr(T* ptr) 
            : ptr(ptr)
            , deleter(Deleter())
    {}

    UniquePtr(T* ptr, Deleter deleter)
            : ptr(ptr)
            , deleter(deleter)
    {}

    // in unique_ptr deleter is called only if ptr != nullptr
    ~UniquePtr() { if (ptr) deleter(ptr); }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other)
            : ptr(other.ptr)
            , deleter(std::move(other.deleter))
    {
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other)
    {
        if (&other != this)
        {
            deleter(ptr);
            ptr = other.ptr;
            deleter = std::move(other.deleter);
            other.ptr = nullptr;
        }
        return *this;
    }

    T* release() noexcept 
    {
        deleter(ptr);
        ptr = nullptr;
        return ptr;
    }

    void reset(T* ptr_ = nullptr) noexcept
    {
        deleter(ptr);
        ptr = ptr_;
    }

    void swap(UniquePtr& other) noexcept
    {
        std::swap(ptr, other.ptr);
        std::swap(deleter, other.deleter);
    }

    T* get() const noexcept { return ptr; }
    Deleter& get_deleter() noexcept { return deleter; }
    const Deleter& get_deleter() const noexcept { return deleter; }
    explicit operator bool () const noexcept { return ptr; }
    T& operator*() const noexcept { return *ptr; }
    T* operator->() const noexcept { return ptr; }
};

template <typename T, typename Deleter>
std::ostream& operator<<(std::ostream& out, const UniquePtr<T, Deleter>& uptr)
{
    out << *uptr;
    return out;
}

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args)
{
    return UniquePtr<T>(new T(std::forward<Args>(args)...));    
}

void* operator new(size_t n)
{ 
    void* ret = malloc(n);
    std::cout << n << " bytes allocated at " << ret << '\n';
    return ret;
}

void operator delete(void* ptr, size_t) 
{ 
    std::cout << "delete at " << ptr << '\n';
    free(ptr); 
}

int main()
{
    UniquePtr<std::string> ptr = makeUnique<std::string>(10, 'a');
    std::cout << ptr << std::endl;
}
