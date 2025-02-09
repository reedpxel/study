#include "deque.hpp"

#include <iostream>
#include <deque>
#include <vector>
#include <thread>
#include <set>
#include <unordered_set>
#include <memory>
#include <exception>

static size_t newCount = 0;

void* operator new(size_t count)
{
//    if (newCount == 4) throw std::bad_alloc();
    ++newCount;
    void* ptr = malloc(count);
    std::cout << count << " bytes allocated at " << ptr << std::endl;
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    std::cout << "delete call at " << ptr << std::endl;
    free(ptr);
}

void operator delete(void* ptr, size_t) noexcept
{
    std::cout << "delete call at " << ptr << std::endl;
    free(ptr);
}

void operator delete[](void* ptr, size_t) noexcept
{
    std::cout << "delete[] call" << std::endl;
    free(ptr);
}

void operator delete[](void* ptr) noexcept
{
    std::cout << "delete[] call" << std::endl;
    free(ptr);
}

struct StructName
{
    static size_t count;

    int* px;
    double* pd;

    StructName() : px(new int(1)), pd(new double(1.1)) 
    {
        std::cout << "StructName()" << std::endl;
    }

    StructName(const StructName&) : px(new int(2)), pd(new double(2.2))
    {
        ++count;
        if (count == 150)
        {
            delete px;
            delete pd;
            std::cout << "throwing exception" << std::endl;
            throw std::invalid_argument("");
        }
        std::cout << "StructName(const StructName&)" << std::endl;
    }
    ~StructName()
    {
        delete px;
        delete pd;
        std::cout << "~StructName()" << std::endl;
    }
};

size_t StructName::count = 0;

struct Test
{
    static size_t count;
    static size_t exceptionCount;
    int* px;
    double* pd;

    Test(int x, double d) : px(new int(x)), pd(new double(d))
    {
        if (count == exceptionCount)
        {
            std::cout << "x is " << exceptionCount << ", throwing from ctor"
                << std::endl;
            delete px;
            delete pd;
            throw std::invalid_argument("");
        }
        ++count;
        std::cout << "Test(), count = " << count << std::endl;
    }

    Test(const Test& other) : px(new int(*other.px)), pd(new double(*other.pd)) 
    {
        if (count == exceptionCount)
        {
            std::cout << "x is " << exceptionCount << 
                ", throwing from copy ctor" << std::endl;
            delete px;
            delete pd;
            throw std::invalid_argument("");
        }
        ++count;
        std::cout << "Test(const Test&), count = " << count << std::endl;
    }

    ~Test()
    {
        delete px;
        delete pd;
        --count;
        std::cout << "~Test(), count = " << count << std::endl;
    }
};

size_t Test::count = 0;
size_t Test::exceptionCount = 50;

struct CopyMove
{
    CopyMove() { std::cout << "Ctor" << std::endl; }
    CopyMove(const CopyMove&) noexcept { std::cout << "Copy" << std::endl; }
    CopyMove(CopyMove&&) { std::cout << "Move" << std::endl; }
    ~CopyMove() { std::cout << "Dtor" << std::endl; }
};

int main()
{
    deque<CopyMove> deque1;
    for (int i = 0; i < 5; ++i) deque1.emplace_back();
    deque1.insert(deque1.begin(), CopyMove());
}

// shrink_to_fit and resize tests
//int main()
//{
//    for (size_t i = 0; i < 1000; ++i)
//    {
//        bool testFailed = false;
//        for (size_t j = 0; j < 1000; ++j)
//        {
//            deque<int> d(i, 1);
//            d.shrink_to_fit();
//            d.resize(j, 2);
//            std::cout << i << ' ' << j;
//            if (d.size() != j || d.end() - d.begin() != j)
//            {
//                testFailed = true;
//                std::cout << " test failed" << std::endl;
//                for (auto x : d) std::cout << x << ' ';
//                std::cout << std::endl;
//                d.printFields();
//                d.printBuckets();
//                break;
//            }
//            std::cout << std::endl;
//        }
//        if (testFailed) break;
//    }
//}

// Swap test. Iterators must not invalidate after swap.

//int main()
//{
//    deque<int> d1(10, 1);
//    deque<int> d2(20, 2);
//    auto it1 = d1.begin();
//    auto it2 = d2.begin();
//    d1.swap(d2);
//    *it1 = 10;
//    *it2 = 20;
//    for (auto x : d1) std::cout << x << ' ';
//    std::cout << std::endl;
//    for (auto x : d2) std::cout << x << ' ';
//    std::cout << std::endl;
//
//}

//int main()
//{
//    deque<int> d(50);
//    for (int i = 0; i < d.size(); ++i) d[i] = i;
//    for (auto it = d.begin(); it != d.end(); ++it) std::cout << *it << ' ';
//    std::cout << std::endl;
//    for (deque<int>::const_iterator cIt = d.cbegin(); cIt != d.cend(); ++cIt)
//    {
//        std::cout << *cIt << ' ';
//    }
//    std::cout << std::endl;
//    for (deque<int>::reverse_iterator rIt = d.rbegin(); rIt != d.rend(); ++rIt)
//    {
//        std::cout << *rIt << ' ';
//    }
//    std::cout << std::endl;
//    for (deque<int>::const_reverse_iterator crIt = d.crbegin();
//        crIt != d.crend(); ++crIt)
//    {
//        std::cout << *crIt << ' ';
//    }
//    std::cout << std::endl;
//}

// operator= test

//int main()
//{
//    deque<Test> d1(10, Test(1, 2.1));
//    deque<Test> d2(1, Test(0, 0.1));
//    try
//    {
//        d2 = d1;
//    } catch (...) {
//        std::cout << "caught" << std::endl;
//    }
//    d1.printBuckets();
//    d2.printBuckets();
//}

// ctor and copy ctor exception handling test

//int main()
//{
//    try
//    {
//        deque<Test> d(10, Test(1, 2.3));
//        d.printBuckets();
//        deque<Test> d2 = d;
//    } catch (...) {
//        std::cout << "caught" << std::endl;
//    }
//}

//int main()
//{
//    deque<double> d = {1, 2, 3};
//    d.push_front(-1);
//    d.insert(1 + d.begin());
//    for (deque<double>::reverse_iterator ri = d.rbegin(); ri != d.rend(); ++ri)
//    {
//        std::cout << *ri;
//    }
//    std::cout << '\n';
//    for (deque<double>::const_reverse_iterator cri = d.crbegin();
//        cri != d.crend(); ++cri)
//    {
//        std::cout << *cri;
//    }
//    std::cout << '\n';
//    const deque<double> cd = d;
//    for (deque<double>::const_reverse_iterator cri = cd.crbegin(); 
//        cri != cd.crend(); ++cri)
//    {
//        std::cout << *cri;
//    }
//    std::cout << '\n';
//}

//int main()
//{
//    deque<int> d1 = {1, 2, 3, 4, 5};
//    deque<int> d2 = {1, 2, 3};
//    std::cout << (d1 == d2) << ' ' << (d1 != d2) << ' ' << (d1 < d2) << ' ' 
//        << (d1 > d2) << std::endl;
//    d2.push_back(4);
//    d2.push_back(5);
//    std::cout << (d1 == d2) << ' ' << (d1 != d2) << ' ' << (d1 < d2) << ' ' 
//        << (d1 > d2) << std::endl;
//    d2.back() = 0;
//    std::cout << (d1 == d2) << ' ' << (d1 != d2) << ' ' << (d1 < d2) << ' ' 
//        << (d1 > d2) << std::endl;
//}

//int main()
//{
//    deque<int> d(32);
//    std::cout << (d.begin() == d.end()) << ' ' << 
//        (d.begin() < d.end()) << ' '  
//        << (d.begin() > d.end()) << ' ' 
//        << (d.begin() != d.end());
//    deque<int> d2(10);
//    std::cout << (d.begin() == d2.begin()) << (d.begin() != d2.begin()) <<
//        (d.begin() < d2.begin()) << (d.begin() > d2.begin());
//}

//int main()
//{
//    deque<double, std::allocator<double>> d1(500);
//    std::cout << d1.empty() << '\n';
//    for (double i = 0; i < 500; ++i)
//    {
//        d1.at(i) = i;
//    }
//    d1.front() = INT_MAX;
//    d1.back() = INT_MIN;
//    for (auto x : d1)
//    {
//        std::cout << x << ' ';
//    }
//    std::cout << '\n';
//    typename deque<double, std::allocator<double>>::iterator it = d1.begin();
//    std::cout << *(it + 6) << ' ' << *(5 + it) << '\n';
//}
