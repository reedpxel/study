#include <iostream>
#include <vector>
#include <unordered_set>
#include <iterator>
#include <type_traits>

template <typename It>
typename std::iterator_traits<It>::difference_type
    distance_(It first, It last)
{
    if constexpr (std::is_base_of_v<
        std::random_access_iterator_tag,
        typename std::iterator_traits<It>::iterator_category>)
    // - почему проверка constexpr?
    // - во-первых, иначе не скопилируется, если передать ниже random access
    // iterator, потому что компилятор без constexpr будет пытаться 
    // скомпилировать часть, в которую при таком типе программа не зайдет.
    // во-вторых, это быстрее, а информация о типе доступна при компиляции
    //
    // std::random_access_iterator_tag и другие теги наследуются друг от друга,
    // поэтому проверка "ниже или выше по категориям итераторов" делается через
    // is_base_of
    // До С++17, когда не было if constexptr, это делалось через перегрузку
    // шаблонных функций, в которую передавался тег random_access итератора.
    {
        std::cout << "distance is count with substraction\n";
        return last - first;
    }
    std::cout << "distance is count with increment\n";
    size_t count = 0;
    for (; first != last; ++first, ++count);
    return count;
}

template <typename It>
typename std::iterator_traits<It>::difference_type
    distance_cpp14_helper(It first, It last, std::input_iterator_tag)
{
    std::cout << "distance is count with increment\n";
    size_t count = 0;
    for (; first != last; ++count, ++first);
    return count;
}

template <typename It>
typename std::iterator_traits<It>::difference_type
    distance_cpp14_helper(It first, It last, std::random_access_iterator_tag)
{
    std::cout << "distance is count with substraction\n";
    return last - first;
}

template <typename It>
typename std::iterator_traits<It>::difference_type
    distance_cpp14_helper(It first, It last, std::contiguous_iterator_tag)
{
    std::cout << "distance is count with substraction\n";
    return last - first;

}
template <typename It>
typename std::iterator_traits<It>::difference_type
    distance_cpp14(It first, It last)
{
    return distance_cpp14_helper(first, last, 
        typename std::iterator_traits<It>::iterator_category());
}

int main()
{
    std::vector<int> v(10, 2);
    std::cout << distance_cpp14(v.begin(), v.end()) << '\n';
    std::unordered_set<int> set_;
    for (int i = 0; i < 100; ++i) set_.insert(i);
    std::cout << distance_cpp14(set_.begin(), set_.end()) << '\n';
}
