#ifndef VARIANT_H
#define VARIANT_H

#include <cstddef>
#include <algorithm>
#include "max_sizeof.hpp"
#include "max_alignof.hpp"
#include "get_index_by_type.hpp"
#include "get_type_by_index.hpp"
#include "is_nullptr.hpp"
#include <iostream> // TO DO: remove

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

template <typename T, typename... Types>
struct VariantAlternative
{
    static constexpr size_t index = get_index_by_type_v<T, Types...>;

    VariantAlternative() = default;

    // TO DO: add copy initialization:
    // std::variant<int, std::string, std::vector<int>> var1 = "abc"; 
    // works, but with this variant only direct initialization is available
    VariantAlternative(const T& value) 
    { 
        construct(value); 
    }

    VariantAlternative(T&& value) 
    {
        construct(std::move(value)); 
    }

    VariantAlternative(const variant<Types...>& other)
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        if (other.activeIndex == index)
        {
            std::cout << "copy after if \n";
            new (thisVariant->buffer) T(*reinterpret_cast<const T*>(
                other.buffer));
            thisVariant->activeIndex = other.activeIndex;
        }
    }

    VariantAlternative(variant<Types...>&& other)
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        if (other.activeIndex == index)
        {
            std::cout << "move after if\n";
            new (thisVariant->buffer) T(std::move(reinterpret_cast<T&>(
                other.buffer)));
            thisVariant->activeIndex = other.activeIndex;
        }
    }

    void construct(const T& value)
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        new (reinterpret_cast<T*>(thisVariant->buffer)) T(value);
        thisVariant->activeIndex = get_index_by_type_v<T, Types...>;
    }

    void construct(T&& value)
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        new (reinterpret_cast<T*>(thisVariant->buffer)) T(std::move(value));
        thisVariant->activeIndex = get_index_by_type_v<T, Types...>;
    }

    variant<Types...>& operator=(const T& value) 
    { 
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        thisVariant->callAllVariantAlternativeDestroy();
        new (reinterpret_cast<T*>(thisVariant->buffer)) T(value);
        thisVariant->activeIndex = get_index_by_type_v<T, Types...>;
        return *thisVariant;
    }
    
    variant<Types...>& operator=(T&& value) 
    { 
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        thisVariant->callAllVariantAlternativeDestroy();
        new (reinterpret_cast<T*>(thisVariant->buffer)) T(std::move(value));
        thisVariant->activeIndex = get_index_by_type_v<T, Types...>;
        return *thisVariant;
    }

    void destroy()
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

    variant() { activeIndex = static_cast<size_t>(-1); }

    variant(const variant& other) 
            : VariantAlternative<Types, Types...>{other}...
    {}

    variant(variant&& other)
            : VariantAlternative<Types, Types...>{std::move(other)}...
    {}

    void callAllVariantAlternativeDestroy()
    {
        (VariantAlternative<Types, Types...>::destroy(), ...);
    }
    
    constexpr size_t index() const noexcept { return activeIndex; }
    
    constexpr bool valueless_by_exception() const noexcept
    {
        return activeIndex == static_cast<size_t>(-1);
    }
    
    ~variant() { callAllVariantAlternativeDestroy(); }
private:
    // fields are not initialized, because they are initialized in
    // VariantAlternative ctors, otherwise initialization of the fields would
    // overlap the data that what was written in VariantAlternative ctor
    alignas(max_alignof_v<Types...>) char buffer[max_sizeof_v<Types...>];
    size_t activeIndex;
};

#endif // VARIANT_H
