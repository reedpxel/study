#include <iostream>
#include <vector>
#include <set>
#include "../Deque/deque.hpp"
#include "../unordered_map/unordered_map.hpp"
#include <boost/pool/pool_alloc.hpp>
#include "stack_allocator.hpp"

template <typename Alloc>
void stdVectorTest(const Alloc& alloc = Alloc())
{
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int, Alloc> v(alloc);
    for (int i = 0; i < 100'000; ++i) v.push_back(i);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> res = end - start;
    std::cout << res.count() << std::endl;
}

template <typename Alloc>
void stdSetTest(const Alloc& alloc = Alloc())
{
    auto start = std::chrono::high_resolution_clock::now();
    std::set<int, std::less<int>, Alloc> v(alloc);
    for (int i = 0; i < 100'000; ++i) v.insert(i);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> res = end - start;
    std::cout << res.count() << std::endl;
}

template <typename Alloc>
void unorderedMapTest(const Alloc& alloc = Alloc())
{
    auto start = std::chrono::high_resolution_clock::now();
    unordered_map<int, int, HashFunctor<int>, EqualFunctor<int>, Alloc> v(
        alloc);
    for (int i = 0; i < 10'000; ++i) v.insert(std::make_pair(i, i));
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> res = end - start;
    std::cout << res.count() << std::endl;
}

template <typename Alloc>
void dequeTest(const Alloc& alloc = Alloc())
{
    auto start = std::chrono::high_resolution_clock::now();
    deque<int, Alloc> d(alloc);
    for (int i = 0; i < 100'000; ++i) d.push_back(i);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> res = end - start;
    std::cout << res.count() << std::endl;
}

int main()
{
    stdVectorTest<std::allocator<int>>(); 
    stdVectorTest<boost::fast_pool_allocator<int>>();
    {
        std::array<char, 8'000'000> arr;
        StackAllocatorArrayWrapper<char, 8'000'000> alloc{arr};
        stdVectorTest<StackAllocatorArrayWrapper<int, 8'000'000>>(alloc); 
    }

    std::cout << std::endl;

    stdSetTest<std::allocator<int>>(); 
    stdSetTest<boost::fast_pool_allocator<int>>();
    {
        std::array<char, 8'000'000> arr;
        StackAllocatorArrayWrapper<char, 8'000'000> alloc{arr};
        stdSetTest<StackAllocatorArrayWrapper<int, 8'000'000>>(alloc);
    }

    std::cout << std::endl;

    unorderedMapTest<std::allocator<unordered_map<int, int>::value_type>>();
    unorderedMapTest<boost::fast_pool_allocator<unordered_map<int, int>::
        value_type>>();
    {
        std::array<char, 8'000'000> arr;
        StackAllocatorArrayWrapper<char, 8'000'000> alloc{arr};
        unorderedMapTest(alloc);
    }

    std::cout << std::endl;

    dequeTest<std::allocator<int>>(); 
    dequeTest<boost::fast_pool_allocator<int>>(); 
    {
        std::array<char, 8'000'000> arr;
        StackAllocatorArrayWrapper<char, 8'000'000> alloc{arr};
        dequeTest<StackAllocatorArrayWrapper<int, 8'000'000>>(alloc); 
    }
}
