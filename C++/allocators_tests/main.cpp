#include <iostream>
#include <vector>
#include <set>
#include "../Deque/deque.hpp"
#include "../unordered_map/unordered_map.hpp"
#include <boost/pool/pool_alloc.hpp>
#include "stack_allocator.hpp"

template <typename Alloc>
void stdVectorTest()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int, Alloc> v;
    for (int i = 0; i < 100'000; ++i) v.push_back(i);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> res = end - start;
    std::cout << res.count() << std::endl;
}

template <typename Alloc>
void stdSetTest()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::set<int, std::less<int>, Alloc> v;
    for (int i = 0; i < 100'000; ++i) v.insert(i);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> res = end - start;
    std::cout << res.count() << std::endl;
}

template <typename Alloc>
void unorderedMapTest()
{
    auto start = std::chrono::high_resolution_clock::now();
    unordered_map<int, int, HashFunctor<int>, EqualFunctor<int>, Alloc> v;
    for (int i = 0; i < 10'000; ++i) v.insert(std::make_pair(i, i));
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> res = end - start;
    std::cout << res.count() << std::endl;
}

int main()
{
//    stdVectorTest<std::allocator<int>>();
//    stdVectorTest<boost::fast_pool_allocator<int>>();
//    stdVectorTest<stack_allocator<int, 8'000'000>>();
//
//    std::cout << std::endl;
//
//    stdSetTest<std::allocator<int>>();
//    stdSetTest<boost::fast_pool_allocator<int>>();
//    stdSetTest<stack_allocator<int, 8'000'000>>();
//
//    std::cout << std::endl;
//
//    unorderedMapTest<std::allocator<unordered_map<int, int>::value_type>>();
//    unorderedMapTest<boost::pool_allocator<unordered_map<int, int>::value_type>>();
//    unorderedMapTest<stack_allocator<unordered_map<int, int>::value_type, 4'000'000>>();

    unordered_map<int, int, HashFunctor<int>, EqualFunctor<int>,
        stack_allocator<std::pair<const int, int>, 4'000'000>> map;
    map.insert(make_pair(1, 1));

}
