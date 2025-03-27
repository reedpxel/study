#include <algorithm>

template <typename... Types>
struct max_alignof;

template <typename Head, typename... Tail>
struct max_alignof<Head, Tail...>
{
    static constexpr size_t value = std::max(alignof(Head), 
        max_alignof<Tail...>::value);
};

template <typename T>
struct max_alignof<T> : std::integral_constant<size_t, alignof(T)> {};

template <typename... Types>
constexpr size_t max_alignof_v = max_alignof<Types...>::value;
