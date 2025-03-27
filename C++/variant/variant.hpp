#ifndef VARIANT_H
#define VARIANT_H

#include <cstddef>
#include <algorithm>
#include "max_sizeof.hpp"
#include "max_alignof.hpp"
#include "get_index_by_type.hpp"
#include "is_nullptr.hpp"
#include <bitset> // TO DO: remove
#include <iostream> // TO DO: remove

template <typename... Types>
class variant;

// TO DO: check that all methods' signatures are the same as in
// cppreference.com, especially that constexpr is placed everywhere

struct bad_variant_access : std::exception {};

template <typename T, typename... Types>
struct VariantAlternative
{
    static constexpr size_t index = get_index_by_type_v<T, Types...>;

    VariantAlternative() = default;
    VariantAlternative(const T& value) { construct(value); }
    VariantAlternative(T&& value) { construct(std::move(value)); }

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
    friend constexpr T get(variant& var)
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

    variant() = default;

    ~variant()
    {
        (VariantAlternative<Types, Types...>::destroy(), ...);
    }
private:
    alignas(max_alignof_v<Types...>) char buffer[max_sizeof_v<Types...>];
    size_t activeIndex = 0;
};

#endif // VARIANT_H
