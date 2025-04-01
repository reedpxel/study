#ifndef VARIANT_H
#define VARIANT_H

#include <cstddef>
#include <algorithm>
#include "max_sizeof.hpp"
#include "max_alignof.hpp"
#include "get_index_by_type.hpp"
#include "get_type_by_index.hpp"

template <typename... Types>
class variant;

struct bad_variant_access : std::exception 
{
    const char* what() const noexcept override
    {
        return "Function \"get\" template parameter and the type currently "
            "stored by variant are not same.\n";
    }
};

// TO DO: add exception handling(make methods constexpr(...))
// TO DO: implement visit

template <typename T, typename... Types>
struct VariantAlternative
{
    static constexpr size_t index = get_index_by_type_v<T, Types...>;

    VariantAlternative() = default;

    // TO DO: add copy initialization:
    // std::variant<int, std::string, std::vector<int>> var1 = "abc"; 
    // works, but with this variant only direct initialization is available
    constexpr VariantAlternative(const T& value) 
    { 
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        new (reinterpret_cast<T*>(thisVariant->buffer)) T(value);
        thisVariant->activeIndex = get_index_by_type_v<T, Types...>;
    }

    constexpr VariantAlternative(T&& value) 
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        new (reinterpret_cast<T*>(thisVariant->buffer)) T(std::move(value));
        thisVariant->activeIndex = get_index_by_type_v<T, Types...>;
    }

    constexpr VariantAlternative(const variant<Types...>& other)
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        if (other.activeIndex == index)
        {
            new (thisVariant->buffer) T(*reinterpret_cast<const T*>(
                other.buffer));
            thisVariant->activeIndex = other.activeIndex;
        }
    }

    constexpr VariantAlternative(variant<Types...>&& other)
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        if (other.activeIndex == index)
        {
            new (thisVariant->buffer) T(std::move(reinterpret_cast<T&>(
                other.buffer)));
            thisVariant->activeIndex = other.activeIndex;
        }
    }

    constexpr variant<Types...>& operator=(const T& value) 
    { 
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        thisVariant->callAllVariantAlternativeDestroy();
        new (reinterpret_cast<T*>(thisVariant->buffer)) T(value);
        thisVariant->activeIndex = get_index_by_type_v<T, Types...>;
        return *thisVariant;
    }
    
    constexpr variant<Types...>& operator=(T&& value) 
    { 
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        thisVariant->callAllVariantAlternativeDestroy();
        new (reinterpret_cast<T*>(thisVariant->buffer)) T(std::move(value));
        thisVariant->activeIndex = get_index_by_type_v<T, Types...>;
        return *thisVariant;
    }

    constexpr void operator=(const variant<Types...>& other)
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        if (!index)
        {
            thisVariant->callAllVariantAlternativeDestroy();
        }
        if (other.activeIndex == index)
        {
            new (thisVariant->buffer) T(*reinterpret_cast<const T*>(
                other.buffer));
            thisVariant->activeIndex = other.activeIndex;
        }
    }

    constexpr void operator=(variant<Types...>&& other)
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        if (!index)
        {
            thisVariant->callAllVariantAlternativeDestroy();
        }
        if (other.activeIndex == index)
        {
            new (thisVariant->buffer) T(std::move(reinterpret_cast<T&>(
                other.buffer)));
            thisVariant->activeIndex = other.activeIndex;
            other.callAllVariantAlternativeDestroy();
            other.activeIndex = static_cast<size_t>(-1);
        }
    }

    constexpr void destroy()
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        if (thisVariant->activeIndex == index)
        {
            reinterpret_cast<T*>(thisVariant->buffer)->~T();
        }
    }
};

template <typename... Types>
class variant : private VariantAlternative<Types, Types...>...
{
public:
    // Adding a ctor
    //
    // template <typename U>
    // variant(const U& value);
    //
    // is not an option, because U may be not only the exact type, but also a
    // convertion or promotion to one of the types. One may write a require
    // expression std::is_convertible<Types...>, but to what type must it be
    // convertible?
    // The solution is to inherit all ctors from VariantAlternative<Types>...

    template <typename T, typename... OtherTypes>
    friend struct VariantAlternative;

    template <typename T>
    friend constexpr T& get(variant& var)
    {
        static_assert(
            get_index_by_type_v<T, Types...> != sizeof...(Types));
        if (get_index_by_type_v<T, Types...> != var.activeIndex)
        {
            throw bad_variant_access{};
        }
        return *reinterpret_cast<T*>(var.buffer);
    }

    using VariantAlternative<Types, Types...>::VariantAlternative...;
    using VariantAlternative<Types, Types...>::operator=...;

    constexpr variant() { activeIndex = static_cast<size_t>(-1); }

    constexpr variant(const variant& other) 
            : VariantAlternative<Types, Types...>{other}...
    {}

    constexpr variant(variant&& other)
            : VariantAlternative<Types, Types...>{std::move(other)}...
    {}

    constexpr variant& operator=(const variant& other)
    {
        if (this == &other) return *this;
        (VariantAlternative<Types, Types...>::operator=(other), ...);
        return *this;
    }

    constexpr variant& operator=(variant&& other)
    {
        if (this == &other) return *this;
        (VariantAlternative<Types, Types...>::operator=(std::move(other)), 
            ...);
        return *this;
    }

    constexpr void callAllVariantAlternativeDestroy()
    {
        (VariantAlternative<Types, Types...>::destroy(), ...);
    }
    
    constexpr size_t index() const noexcept { return activeIndex; }
    
    constexpr bool valueless_by_exception() const noexcept
    {
        return activeIndex == static_cast<size_t>(-1);
    }
    
    constexpr ~variant() { callAllVariantAlternativeDestroy(); }
private:
    // fields are not initialized, because they are initialized in
    // VariantAlternative ctors, otherwise initialization of the fields would
    // overlap the data that what was written in VariantAlternative ctor
    alignas(max_alignof_v<Types...>) char buffer[max_sizeof_v<Types...>];
    size_t activeIndex;
};

#endif // VARIANT_H
