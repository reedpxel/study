#include "function.hpp"
#include <functional>
#include <cmath>

struct BigObject
{
    size_t x1 = 1;
    size_t x2 = 2;
    size_t x3 = 3;
    void operator()() const { std::cout << x1 << x2 << x3 << '\n'; }
};

struct SmallObject
{
    void operator()() const { std::cout << "small" << '\n'; }
};

struct Object16Bytes
{
    size_t x1 = 1;
    size_t x2 = 2;
    void operator()() const { std::cout << x1 << x2 << std::endl; }
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
void cStyleFunction2() { std::cout << 456 << '\n'; }

int main()
{
    auto f = []() { std::cout << 1; };
    std::reference_wrapper<decltype(f)> wr(f);
    function<void()> func = wr;
    func();
}

//int main()
//{
//    auto l = [](int x)
//    {
//        std::cout << x << '\n';
//        return x * x;
//    };
//    function<int()> f = std::bind(l, 100);
//    std::cout << f() << '\n';
//}

//struct S
//{
//    int field = 9;
//    int method(double d)
//    {
//        std::cout << d << " method\n";
//        return 1;
//    }
//};
//
//int main()
//{
//    S s;
//    function<int(S&, double)> fMethod = &S::method;
//    fMethod(s, 1.1);
//    function<int(S&)> fField = &S::field;
//    std::cout << fField(s) << '\n';
//}

//int main()
//{
//    function<int(void*)> f;
//    std::cout << f.target_type().name() << std::endl;
//    std::cout << f.target<void>() << std::endl;
//    function<int(void*)> f2 = [](void*)
//    {
//        return 0;
//    };
//    std::cout << f2.target_type().name() << std::endl;
//    std::cout << f.target<void>() << std::endl;
//}

//int main()
//{
//    function<int(double)> f1 = [](double d) -> int
//    {
//        std::cout << "1 " << d << std::endl;
//        return ceil(sqrt(d));
//    };
//    function<int(double)> f2 = [](double d) -> int
//    {
//        std::cout << "2 " << d << std::endl;
//        return floor(d * d);
//    };
//    f1.swap(f2);
//    std::cout << f1(1.1) << '\n' << f2(2.2) << '\n';
//    // 2 1.1 1 ___ 1 2.2 2
//    function<void()> f3 = BigObject();
//    function<void()> f4 = SmallObject();
//    f3.swap(f4);
//    f3();
//    f4();
//}

//int main()
//{
//    function<void()> f = [] { std::cout << "abcd" << std::endl; };
//    f();
//    f = BigObject();
//    f();
//    f = SmallObject();
//    f();
//    f = [] { std::cout << "defg" << std::endl; };
//    f();
//    f = cStyleFunction;
//    f();
//    f = &cStyleFunction2;
//    f();
//    f = nullptr;
//    try
//    {
//        f();
//    } catch(...) {
//        std::cout << "caught\n";
//    }
//}

//int main()
//{
//    function<void()> f = BigObject();
//    function<void()> f2 = std::move(f);
//    f2();
//}

//int main()
//{
//    function<void()> f1 = [] { std::cout << 123 << '\n'; };
//    function<void()> f2 = BigObject();
//    function<void()> f3 = SmallObject();
//    function<void()> f4 = &cStyleFunction;
//    function<void()> f5 = cStyleFunction;
//    function<void()> fTest1 = f1;
//    fTest1();
//    function<void()> fTest2 = f2;
//    fTest2();
//    function<void()> fTest3 = f3;
//    fTest3();
//    function<void()> fTest4 = f4;
//    fTest4();
//    function<void()> fTest5 = f5;
//    fTest5();
//    std::cout << std::endl;
//    function<void()> fTest6 = std::move(f1);
//    fTest6();
//    function<void()> fTest7 = std::move(f2);
//    fTest7();
//    function<void()> fTest8 = std::move(f3);
//    fTest8();
//    function<void()> fTest9 = std::move(f4);
//    fTest9();
//    function<void()> fTest10 = std::move(f5);
//    fTest10();
//    std::cout << std::endl;
//    try
//    {
//        f1();
//    } catch (...) {
//        std::cout << "Exception caught\n";
//    }
//    try
//    {
//        f2();
//    } catch (...) {
//        std::cout << "Exception caught\n";
//    }
//    try
//    {
//        f3();
//    } catch (...) {
//        std::cout << "Exception caught\n";
//    }
//    try
//    {
//        f4();
//    } catch (...) {
//        std::cout << "Exception caught\n";
//    }
//    try
//    {
//        f5();
//    } catch (...) {
//        std::cout << "Exception caught\n";
//    }
//}

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

