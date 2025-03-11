#include "function.hpp"

struct BigObject
{
    size_t x1 = 1;
    size_t x2 = 2;
    size_t x3 = 3;
    void operator()() const { std::cout << x1 << x2 << x3 << '\n'; }
};

void* operator new(size_t n)
{ 
    void* ret = malloc(n);
    std::cout << n << " bytes allocated at " << ret << std::endl;
    return ret;
}

void operator delete(void* pObject)
{
    std::cout << "delete at " << pObject << std::endl;
    free(pObject);
}

void operator delete(void* pObject, size_t n) 
{ 
    std::cout << "delete " << n << " bytes at " << pObject << std::endl;
    free(pObject); 
}

void cStyleFunction() { std::cout << 123 << '\n'; }

int main()
{
    function<void()> func1 = BigObject();
    func1();
    function<void()> func2 = []{ std::cout << 123; };
    func2();
    function<void()> func3 = cStyleFunction();
    func3();
}

