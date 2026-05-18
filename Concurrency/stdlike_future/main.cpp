#include <iostream>
#include <vector>
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

void test4()
{
    Promise<std::exception_ptr> p;
    Future<std::exception_ptr> f = p.getFuture();
    std::string exceptionMessage{"Error"};

    std::thread{[&p, &exceptionMessage] {
        std::this_thread::sleep_for(1s);
        p.setException(std::make_exception_ptr(std::logic_error{
            exceptionMessage}));
    }}.detach();
    try
    {
        std::exception_ptr eptr = f.get();
    }
    catch (const std::exception& exc)
    {
        assert(exc.what() == exceptionMessage);
        std::cout << "Test 4 passed" << std::endl;
    }
}

void test5()
{
    {
        Promise<int> p;
    }
    std::cout << "Test 5 passed" << std::endl;
}

void test6()
{
    {
        Promise<double> p;
        Future<double> f = p.getFuture();
    }
    std::cout << "Test 6 passed" << std::endl;
}

void test7()
{
    Promise<int>* p = new Promise<int>{};
    Future<int>* f = new Future<int>(p->getFuture());
    std::thread{[p] 
        {
            p->setValue(5);
            delete p;
        }}.detach();
    std::this_thread::sleep_for(1s);
    assert(f->get() == 5);
    delete f;
    std::cout << "Test 7 passed" << std::endl;
}

void test8()
{
    {
        Promise<char> p;
        Future<char> f = p.getFuture();
        std::thread{[&p] 
            {
                p.setValue(6);
            }}.join();
    }
    std::cout << "Test 8 passed" << std::endl;
}

void test9()
{
    {
        Promise<std::vector<std::string>> p;
        p.setValue({"abcd", "efgh"});
    }
    std::cout << "Test 9 passed" << std::endl;
}

void test10()
{
    Promise<std::string> p;
    Future<std::string> f = p.getFuture();
    std::string s{"abcde"};
    std::thread{[&p, &s]() mutable
        {
            std::this_thread::sleep_for(1s);
            p.setValue(std::move(s));
        }}.detach();
    assert(f.get() == "abcde");
    assert(s.empty());
    std::cout << "Test 10 passed" << std::endl;
}

void testCopyOnly()
{
    struct CopyOnly
    {
        CopyOnly() = default;
        CopyOnly(const CopyOnly&) = default;
        CopyOnly& operator=(const CopyOnly&) = default;
        CopyOnly(CopyOnly&&) = delete;
        CopyOnly& operator=(CopyOnly&&) = delete;
    };
    Promise<CopyOnly> p;
    Future<CopyOnly> f = p.getFuture();

    std::thread{[&p] 
        {
            std::this_thread::sleep_for(1s);
            CopyOnly res{};
            p.setValue(res);
        }}.detach();
    [[maybe_unused]] CopyOnly res = f.get();
    std::cout << "testCopyOnly passed" << std::endl;
}

void testMoveOnly()
{
    struct MoveOnly
    {
        MoveOnly() = default;
        MoveOnly(const MoveOnly&) = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;
        MoveOnly(MoveOnly&&) = default;
        MoveOnly& operator=(MoveOnly&&) = default;
    };
    Promise<MoveOnly> p;
    Future<MoveOnly> f = p.getFuture();

    std::thread{[&p] 
        {
            std::this_thread::sleep_for(1s);
            p.setValue(MoveOnly{});
        }}.detach();
    [[maybe_unused]] MoveOnly res = f.get();
    std::cout << "testMoveOnly passed" << std::endl;
}

void testMoveOpEqual()
{
    Promise<int> p1;
    Future<int> f = p1.getFuture();
    Promise<int> p2;
    p2 = std::move(p1);

    std::thread{[&p2] 
        {
            std::this_thread::sleep_for(1s);
            p2.setValue(7);
        }}.detach();
    assert(f.get() == 7);
    std::cout << "Promise::operator=(Promise&&) test passed" << std::endl;
}

void runAllTests()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    testCopyOnly();
    testMoveOnly();
    testMoveOpEqual();
    std::cout << "All tests passed" << std::endl;
}

int main()
{
    runAllTests();
}

