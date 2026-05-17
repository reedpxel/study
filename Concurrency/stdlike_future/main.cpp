#include <iostream>
#include "future.hpp"

using namespace std::chrono_literals;

void test1()
{
    Promise<int> p;
    Future<int> f = p.getFuture();
    p.setValue(12);
    assert(f.get() == 12);
    std::cout << "Test 1 passed" << std::endl;
}

void test2()
{
    Promise<int> p;
    Future<int> f = p.getFuture();
    std::thread{[&p] 
        {
            std::this_thread::sleep_for(1s);
            p.setValue(123);
        }}.detach();
    assert(f.get() == 123);
    std::cout << "Test 2 passed" << std::endl;
}

void test3()
{
    struct NoDefaultCtor
    {
        NoDefaultCtor(int) {}
    };

    std::string exceptionMessage{"setException() test"};
    Promise<NoDefaultCtor> p;
    Future<NoDefaultCtor> f = p.getFuture();
    std::thread{[&p, &exceptionMessage]
        {
            std::this_thread::sleep_for(1s);
            p.setException(std::make_exception_ptr(
                std::logic_error{exceptionMessage}));
        }}.detach();
    try
    {
        f.get();
    }
    catch (const std::exception& exc)
    {
        assert(exc.what() == exceptionMessage);
        std::cout << "Test 3 passed" << std::endl;
    }
}

void runAllTests()
{
    test1();
    test2();
    test3();
    std::cout << "All tests passed" << std::endl;
}

int main()
{
    runAllTests();
//    std::expected<std::string, std::exception_ptr> exp("abcd");
//    std::cout << exp.value() << std::endl;

//    std::promise<int> p;
//    std::future<int> f = p.get_future();
//    p.set_value(123);
//    std::cout << f.get() << std::endl;
}

