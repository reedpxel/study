#include <array>
#include <exception>
#include <cstdint>
#include <type_traits>

template <typename T, size_t BufferSize>
class stack_allocator
{
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = size_t;
//    using propagate_on_container_copy_assignment = std::false_type;
//    using propagate_on_container_move_assignment = std::false_type;

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
//    struct propagate_on_container_copy_assignment 
//    {
//        const static bool value = true;
//    };
//    struct propagate_on_container_move_assignment
//    {
//        const static bool value = true;
//    };
private:
    char buffer[BufferSize] = {};
    size_t offset = 0;
};
