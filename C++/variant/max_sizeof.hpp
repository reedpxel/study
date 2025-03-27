#include <algorithm>

template <typename... Types>
struct max_sizeof;

template <typename Head, typename... Tail>
struct max_sizeof<Head, Tail...>
{
    static constexpr size_t value = std::max(sizeof(Head), 
        max_sizeof<Tail...>::value);
};

template <typename T>
struct max_sizeof<T> : std::integral_constant<size_t, sizeof(T)> {};

template <typename... Types>
constexpr size_t max_sizeof_v = max_sizeof<Types...>::value;

