#include "weak_ptr.hpp"

#include <iostream>

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

struct Base 
{
    int b;
};

struct Derived : Base 
{
    int d;
};

struct Mom
{
    int m;
    ~Mom() { std::cout << "~Mom" << std::endl; }
};

struct Dad
{
    int d;
    ~Dad() { std::cout << "~Dad" << std::endl; }
};

struct Son : Mom, Dad
{
    int s;
    ~Son() { std::cout << "~Son" << std::endl; }
};


struct S : enable_shared_from_this<S>
{
    size_t sz = 0;
    shared_ptr<S> getSharedPtr() { return shared_from_this(); }
};

int main()
{
    S* pS = new S;
    shared_ptr<S> shptr(pS);
    shared_ptr<S> shptr2 = pS->getSharedPtr();
    std::cout << shptr.use_count() << std::endl;
}

//int main()
//{
//    shared_ptr<Son> shptr = make_shared<Son>(Son{1, 2, 3});
//    shared_ptr<Dad> shptr2 = shptr;
//    std::cout << shptr.get() << ' ' << shptr2.get() << std::endl;
//    shptr.reset();
//}

//int main()
//{
//    {
//        shared_ptr<std::string> shptr;
//        weak_ptr<std::string> wptr = shptr;
//        weak_ptr<std::string> wptr2;
//        wptr = wptr2;
//    }
//    std::cout << "Test 1 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr(new std::string("abc"));
//        weak_ptr<std::string> wptr = shptr;
//        shptr.reset();
//        weak_ptr<std::string> wptr2;
//        wptr2 = shptr;
//        weak_ptr<std::string> wptr3;
//        wptr3 = wptr;
//        weak_ptr<std::string> wptr4;
//        wptr4 = std::move(wptr2);
//        weak_ptr<std::string> wptr5;
//    }
//    std::cout << "Test 2 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr = make_shared<std::string>("abc");
//        auto shptr2 = shptr;
//        weak_ptr<std::string> wptr = shptr;
//        shptr.reset();
//        weak_ptr<std::string> wptr2;
//        wptr2 = shptr;
//        weak_ptr<std::string> wptr3;
//        wptr3 = wptr;
//        weak_ptr<std::string> wptr4;
//        wptr4 = std::move(wptr2);
//        weak_ptr<std::string> wptr5;
//    }
//    std::cout << "Test 3 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr(new std::string("abcd"));
//        shared_ptr<std::string> shptr2 = shptr;
//        weak_ptr<std::string> wptr = shptr;
//        shptr.reset();
//        weak_ptr<std::string> wptr2;
//        wptr2 = shptr;
//        weak_ptr<std::string> wptr3;
//        wptr3 = wptr;
//        weak_ptr<std::string> wptr4;
//        wptr4 = std::move(wptr2);
//        weak_ptr<std::string> wptr5;
//    }
//    std::cout << "Test 4 passed" << std::endl;
//}

//int main()
//{
//    {
//        shared_ptr<std::string> shptr;
//        weak_ptr<std::string> wptr = shptr;
//    }
//    std::cout << "Test 1 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr(new std::string("abcde"));
//        weak_ptr<std::string> wptr = shptr;
//        shptr.reset();
//    }
//    std::cout << "Test 2 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr = make_shared<std::string>("abcd");
//        weak_ptr<std::string> wptr1 = shptr;
//        weak_ptr<std::string> wptr2 = shptr;
//        weak_ptr<std::string> wptr3 = wptr1;
//        shptr.reset();
//    }
//    std::cout << "Test 3 passed" << std::endl;
//    {
//        shared_ptr<std::string> shptr1(new std::string("abccd"));
//        shared_ptr<std::string> shptr2 = shptr1;
//        weak_ptr<std::string> wptr1 = shptr1;
//        weak_ptr<std::string> wptr2 = wptr1;
//    }
//    std::cout << "Test 4 passed" << std::endl;
//}
//
