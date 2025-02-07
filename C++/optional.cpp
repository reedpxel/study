#include <iostream>
#include <cstdint>

struct nullopt_t {};
nullopt_t nullopt;

template <typename T>
class optional
{
public:
    using value_type = T;

    optional() noexcept = default; 
    optional(nullopt_t) noexcept {}; 

    optional(const optional& other) 
            : initialized(other.initialized)
    {
        if (initialized)
        {
            new (reinterpret_cast<T*>(arr)) T(*reinterpret_cast<const T*>(
                other.arr));
        }
    }

    optional(optional&& other) noexcept(std::is_nothrow_move_constructible<
        T>::value) 
            : initialized(other.initialized)
    {
        if (initialized)
        {
             new (reinterpret_cast<T*>(arr)) T(std::move(*reinterpret_cast<T*>(
                other.arr)));
        }
    }

    optional(const T& value) 
            : initialized(true)
    {
        new (reinterpret_cast<T*>(arr)) T(value);
    }

    optional(T&& value) 
            : initialized(true)
    {
        new (reinterpret_cast<T*>(arr)) T(std::move(value));
    }

    ~optional()  
    {
        if (initialized) reinterpret_cast<T*>(arr)->~T();
    }

    constexpr optional& operator=(const optional& other) 
    {
        if (other.initialized)
        {
            if (initialized)
            {
                reinterpret_cast<T*>(arr)->~T();
                new (reinterpret_cast<T*>(arr)) T(*reinterpret_cast<const T*>(
                    other.arr));
            } else {
                new (reinterpret_cast<T*>(arr)) T(*reinterpret_cast<const T*>(
                    other.arr));
                initialized = true;
            }
        } else {
            if (initialized)
            {
                reinterpret_cast<T*>(arr)->~T();
                initialized = false;
            }
        }
        return *this;
    }

    constexpr optional& operator=(optional&& other) noexcept(
        std::is_nothrow_move_assignable_v<T> && 
        std::is_nothrow_move_constructible_v<T>) 
    {
        if (other.initialized)
        {
            if (initialized)
            {
                reinterpret_cast<T*>(arr)->~T();
                new (reinterpret_cast<T*>(arr)) T(std::move(*reinterpret_cast<
                    T*>(other.arr)));
            } else {
                new (reinterpret_cast<T*>(arr)) T(std::move(*reinterpret_cast<
                    T*>(other.arr)));
                initialized = true;
            }
        } else {
            if (initialized)
            {
                reinterpret_cast<T*>(arr)->~T();
                initialized = false;
            }
        }
        return *this;
    }
    // op-> and op* do not check, whether optional has value or not, this check
    // must be done manually with static_cast<bool> or has_value()
    constexpr const T* operator->() const noexcept
    {
        return reinterpret_cast<const T*>(arr);
    }

    constexpr T* operator->() noexcept
    {
        return reinterpret_cast<T*>(arr);
    }

    constexpr const T& operator*() const & noexcept
    {
        return *reinterpret_cast<const T*>(arr);
    }

    constexpr T& operator*() & noexcept
    {
        return *reinterpret_cast<T*>(arr);
    }

    constexpr const T&& operator*() const && noexcept
    {
        return std::move(*reinterpret_cast<T*>(arr));
    }

    constexpr T&& operator*() && noexcept
    {
        return std::move(*reinterpret_cast<T*>(arr));
    }

    explicit operator bool() const noexcept { return initialized; }
    bool hasValue() const noexcept { return initialized; }
    
    constexpr T& value() &
    {
        return *reinterpret_cast<T*>(arr);
    }
    constexpr const T& value() const &
    {
        return *reinterpret_cast<const T*>(arr);
    }

    constexpr T&& value() &&
    {
        return std::move(*reinterpret_cast<T*>(arr));
    }
    
    constexpr const T&& value() const &&
    {
        return std::move(*reinterpret_cast<T*>(arr));
    }

    constexpr T value_or(const T& default_value) const &
    {
        return initialized ? *reinterpret_cast<T*>(arr) : default_value;
    }

    constexpr T value_or(T&& default_value) &&
    {
        return initialized ? std::move(*reinterpret_cast<T*>(arr)) : 
            std::move(default_value);
    }

    void swap(optional& other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&
        std::is_nothrow_swappable_v<T>)
    {
        if (initialized)
        {
            if (other.initialized)
            {
                T tmp = std::move(value());
                *reinterpret_cast<T*>(arr) = std::move(other.value());
                *reinterpret_cast<T*>(other.arr) = std::move(tmp);
            } else {
                new (reinterpret_cast<T*>(other.arr)) T(std::move(value()));
                reinterpret_cast<T*>(arr)->~T();
            }
        } else {
            if (other.initialized)
            {
                new (reinterpret_cast<T*>(arr)) T(std::move(other.value()));
                reinterpret_cast<T*>(other.arr)->~T();
            }
        }
        std::swap(initialized, other.initialized);
    }

    constexpr void reset() noexcept
    {
        if (initialized)
        {
            reinterpret_cast<T*>(arr)->~T();
            initialized = false;
        }
    }

    template <typename... Args>
    T& emplace(Args&&... args)
    {
        if (initialized)
        {
            reinterpret_cast<T*>(arr)->~T();
        }
        new (reinterpret_cast<T*>(arr)) T(std::forward<Args>(args)...);
        initialized = true;
        return value();
    }

    auto operator<=>(const optional& other) const
    {
        return *reinterpret_cast<T*>(arr) <=> *reinterpret_cast<T*>(other.arr);
    }
protected:
    char arr[alignof(T)];
    bool initialized = false;
};

struct S
{
    size_t x;
    bool operator==(const S& other) const noexcept 
    { 
        return x == other.x; 
    }
    S(size_t x) : x(x) 
    {
        std::cout << "S(" << x << ')' << std::endl;
    }
    S(const S& other) : x(other.x)
    {
        std::cout << "S(const S&{" << x << "})" << std::endl;
    }
    S(S&& other) : x(other.x)
    {
        other.x = 0;
        std::cout << "S(S&&{" << x << "})" << std::endl;
    }
    ~S() { std::cout << "~S(" << x << ')'  << std::endl; }
};

int main()
{
    optional<S> opt;
    opt.reset();
    opt.emplace(111);
    opt ?
        std::cout << opt.value().x << ' ' :
        std::cout << "opt has no value ";

}
