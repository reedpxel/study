#include <array>
#include <exception>
#include <cstdint>
#include <type_traits>
#include <memory>

template <typename T, size_t BufferSize>
class stack_allocator
{
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = size_t;

    stack_allocator() = default;

    template <typename U>
    stack_allocator(const stack_allocator<U, BufferSize>&) noexcept {}

    T* allocate(size_t n)
    {
//        std::cout << BufferSize - offset << " bytes remaining, trying to "
//            "allocate " << n * sizeof(T) << " bytes" << std::endl;
        if (n * sizeof(T) + offset >= BufferSize) [[unlikely]]
        {
            throw std::bad_alloc();
        }
        T* res = reinterpret_cast<T*>(buffer + offset);
        offset += n * sizeof(T);
        return res;
    }

    void deallocate(T*, size_t) {}

    template <typename U>
    struct rebind
    {
        using other = stack_allocator<U, BufferSize>;
    };
    struct propagate_on_container_copy_assignment 
    {
        const static bool value = true;
    };
    struct propagate_on_container_move_assignment
    {
        const static bool value = true;
    };
private:
    char buffer[BufferSize] = {};
    size_t offset = 0;
};

template <typename T, size_t BufferSize>
class StackAllocatorArrayWrapper
{
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = size_t;

    StackAllocatorArrayWrapper() = delete;

    StackAllocatorArrayWrapper(std::array<char, BufferSize>& array_)
            : offset(new size_t(0))
            , arr(array_.data())
    {}

    template <typename U>
    StackAllocatorArrayWrapper(const StackAllocatorArrayWrapper<U, 
        BufferSize>& other)
            : offset(other.offset)
            , arr(other.arr)
    {}

    bool operator==(const StackAllocatorArrayWrapper& other) const noexcept
    {
        return arr == other.arr;
    }

    T* allocate(size_t amountOfObjects)
    {
        if (amountOfObjects * std::max(sizeof(T), alignof(T)) + *offset >= 
            BufferSize)
        {
            throw std::bad_alloc();
        }
        T* res = reinterpret_cast<T*>(arr + *offset);
        *offset += amountOfObjects * sizeof(T);
        return res;
    }

    void deallocate(T*, size_t) noexcept {}
    
    template <typename U>
    struct rebind
    {
        using other = StackAllocatorArrayWrapper<U, BufferSize>;
    };

    struct propagate_on_container_copy_assignment 
    {
        const static bool value = true;
    };
    
    struct propagate_on_container_move_assignment
    {
        const static bool value = true;
    };
public:
    std::shared_ptr<size_t> offset;
    char* arr;
};
