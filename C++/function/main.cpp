#include "function.hpp"
#include <functional>

struct BigObject
{
    size_t x1 = 1;
    size_t x2 = 2;
    size_t x3 = 3;
    void operator()() const { std::cout << x1 << x2 << x3 << '\n'; }
};

struct SmallObject
{
    void operator()() const { std::cout << 123 << '\n'; }
};

//void* operator new(size_t n)
//{ 
//    void* ret = malloc(n);
//    std::cout << n << " bytes allocated at " << ret << std::endl;
//    return ret;
//}
//
//void operator delete(void* pObject)
//{
//    std::cout << "delete at " << pObject << std::endl;
//    free(pObject);
//}
//
//void operator delete(void* pObject, size_t n) 
//{ 
//    std::cout << "delete " << n << " bytes at " << pObject << std::endl;
//    free(pObject); 
//}

void cStyleFunction() { std::cout << 123 << '\n'; }

int main()
{
    std::function<void()> f1 = [] { std::cout << 123 << '\n'; };
    std::function<void()> f2 = BigObject();
    std::function<void()> f3 = SmallObject();
    std::function<void()> f4 = &cStyleFunction;
    std::function<void()> f5 = cStyleFunction;
    std::function<void()> fTest1 = f1;
    fTest1();
    std::function<void()> fTest2 = f2;
    fTest2();
    std::function<void()> fTest3 = f3;
    fTest3();
    std::function<void()> fTest4 = f4;
    fTest4();
    std::function<void()> fTest5 = f5;
    fTest5();
    std::cout << std::endl;
    std::function<void()> fTest6 = std::move(f1);
    fTest6();
    std::function<void()> fTest7 = std::move(f2);
    fTest7();
    std::function<void()> fTest8 = std::move(f3);
    fTest8();
    std::function<void()> fTest9 = std::move(f4);
    fTest9();
    std::function<void()> fTest10 = std::move(f5);
    fTest10();
    std::cout << std::endl;
    try
    {
        f1();
    } catch (...) {
        std::cout << "Exception caught\n";
    }
    try
    {
        f2();
    } catch (...) {
        std::cout << "Exception caught\n";
    }
    try
    {
        f3();
    } catch (...) {
        std::cout << "Exception caught\n";
    }
    try
    {
        f4();
    } catch (...) {
        std::cout << "Exception caught\n";
    }
    try
    {
        f5();
    } catch (...) {
        std::cout << "Exception caught\n";
    }
}

//int main()
//{
//    function<void()> f1 = []() { std::cout << 123 << '\n'; };
//    f1();
//    function<void()> f2 = BigObject{};
//    f2();
//    function<void()> f3 = SmallObject{};
//    f3();
//    function<void()> f4 = &cStyleFunction;
//    f4();
//    function<void()> f5 = cStyleFunction;
//    f5();
//}

