template <size_t Index, typename... Types>
struct get_type_by_index;

template <typename Head, typename... Tail>
struct get_type_by_index<0, Head, Tail...> : std::type_identity<Head> {};

template <size_t Index, typename Head, typename... Tail>
struct get_type_by_index<Index, Head, Tail...> :
    std::type_identity<std::conditional_t<
        !Index, Head, typename get_type_by_index<Index - 1, Tail...>::type>> 
{};

template <size_t Index>
struct get_type_by_index<Index> : std::type_identity<void> {};

template <size_t Index, typename... Types>
using get_type_by_index_t = get_type_by_index<Index, Types...>::type;
