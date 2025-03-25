#ifndef VARIANT_H
#define VARIANT_H

#include <cstddef>
#include <algorithm>

#ifndef IS_NULLPTR
#define IS_NULLPTR

template <typename T>
bool isNullptr(T* ptr) 
{
#ifdef __SANITIZE_ADDRESS__
    return ptr == nullptr || ptr == (T*)0xbebebebebebebebe;
#else 
    return ptr == nullptr;
#endif
}

#endif // IS_NULLPTR

template <typename... Types>
struct getIndexByType
{
    static constexpr size_t value = 0;
};

template <typename... Types>
constexpr size_t getIndexByType_v = getIndexByType<Types...>::value;

template <typename... Types>
class variant;

template <typename T, typename... Types>
struct VariantAlternative
{
    static constexpr size_t index = getIndexByType_v<T, Types...>;
    VariantAlternative() = default;
    VariantAlternative(const T& value) 
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        new (thisVariant->buffer) T(value);
        thisVariant->activeIndex = getIndexByType_v<T, Types...>;
    }
    VariantAlternative(T&& value) 
    {
        auto* thisVariant = static_cast<variant<Types...>*>(this);
        new (thisVariant->buffer) T(std::move(value));
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
struct max_sizeof;

template <typename Head, typename... Tail>
struct max_sizeof<Head, Tail...>
{
    static constexpr size_t value = std::max(sizeof(Head), 
        max_sizeof<Tail...>::value);
};

template <typename T>
struct max_sizeof<T> 
{
    static constexpr size_t value = sizeof(T);
};

template <typename... Types>
constexpr size_t max_sizeof_v = max_sizeof<Types...>::value;

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
    // Solution - inheriting all ctors from VariantAlternative<Types>...

    template <typename T, typename... OtherTypes>
    friend struct VariantAlternative;

    using VariantAlternative<Types, Types...>::VariantAlternative...;
    variant() = default;
    ~variant()
    {
        (VariantAlternative<Types, Types...>::destroy(), ...);
    }
private:
    // TO DO: add alignof
    /*alignof(max_alignof_v<Types...>)*/ char buffer[max_sizeof_v<Types...>]{};
    size_t activeIndex = 0;
};

#endif // VARIANT_H
