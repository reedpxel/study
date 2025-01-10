#include <iostream>
#include <mutex>

class Singleton // the object is created in dynamic memory
{
    static Singleton* ptr;
    static std::mutex mutex_;

    Singleton() { std::cout << "Created\n"; }
    ~Singleton() { std::cout << "Destroyed\n"; } 
    
    // Incorrect!
    // Operator delete calls the destructor of the object, the result is
    // recursion and segmentation fault
    /*~Singleton()
    {
        std::cout << "Deleted\n";
        delete ptr; 
    }*/

public:
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    static Singleton& getObject()
    {
        std::lock_guard<std::mutex> lock_guard_(mutex_);
        if (!ptr)
        {
            ptr = new Singleton;
        }
        return *ptr;
    }
    static void destroyObject()
    {
        std::lock_guard<std::mutex> lock_guard_(mutex_);
        if (!ptr) return;
        delete ptr;
        ptr = nullptr; // to not get double free CE
    }
};

Singleton* Singleton::ptr = nullptr;
std::mutex Singleton::mutex_;

struct SegFaultTest
{
    static int count;
    ~SegFaultTest()
    {
        std::cout << "destructor call N " << count++ << '\n';
        delete this; 
    }
};

int SegFaultTest::count = 0;

class SingletonStatic // the object is created in static memory. No mutex is
                      // required, because in C++11 initialization of static
                      // variables is atomic
{
    SingletonStatic() { std::cout << "Created\n"; }
    ~SingletonStatic() { std::cout << "Destroyed\n"; }
public:
    SingletonStatic(const SingletonStatic&) = delete;
    SingletonStatic(Singleton&&) = delete;
    static SingletonStatic& getObject()
    {
        static SingletonStatic object;
        return object;
    }

};

int main()
{
    SingletonStatic& ref = SingletonStatic::getObject();
    SingletonStatic& ref2 = SingletonStatic::getObject();
}
