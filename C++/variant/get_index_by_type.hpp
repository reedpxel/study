template <typename... Types>
struct get_index_by_type;

template <typename T, typename PackHead, typename... PackTail>
struct get_index_by_type<T, PackHead, PackTail...> 
{
    static constexpr size_t value = std::is_same_v<T, PackHead> ?
        0 : get_index_by_type<T, PackTail...>::value + 1;
};

template <typename T>
struct get_index_by_type<T> : std::integral_constant<size_t, 0> {};

template <typename T, typename... Pack>
constexpr size_t get_index_by_type_v = get_index_by_type<T, Pack...>::value;

