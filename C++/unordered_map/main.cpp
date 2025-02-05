#include "unordered_map.hpp"

#include <iostream>

// void* operator new(size_t count)
// {
//     void* ptr = malloc(count);
//     std::cout << count << " bytes allocated at " << ptr << '\n';
//     return ptr;
// }
// 
// void operator delete(void* ptr)
// {
//     std::cout << "deallocation at " << ptr << '\n';
//     free(ptr);
// }
// 
// void operator delete(void* ptr, size_t count)
// {
//     std::cout << count << " bytes deallocated at " << ptr << '\n';
//     free(ptr);
// }

struct S
{
    int x;
    bool operator==(const S& other) const { return x == other.x; }
};

struct KeyType
{
    size_t x;
    bool operator==(const KeyType& other) const noexcept 
    { 
        return x == other.x; 
    }
    KeyType(size_t x) : x(x) 
    {
        std::cout << "KeyType(" << x << ')' << std::endl;
    }
    KeyType(const KeyType& other) : x(other.x)
    {
        std::cout << "KeyType(const KeyType&{" << x << "})" << std::endl;
    }
    KeyType(KeyType&& other) : x(other.x)
    {
        other.x = 0;
        std::cout << "KeyType(KeyType&&{" << x << "})" << std::endl;
    }
    ~KeyType() { std::cout << "~KeyType(" << x << ')'  << std::endl; }
};

struct ValueType
{
    size_t x;
    bool operator==(const ValueType& other) const noexcept 
    { 
        return x == other.x; 
    }
    ValueType(size_t x) : x(x) 
    {
        std::cout << "ValueType(" << x << ')' << std::endl;
    }
    ValueType(const ValueType& other) : x(other.x)
    {
        std::cout << "ValueType(const ValueType&{" << x << "})" << std::endl;
    }
    ValueType(ValueType&& other) : x(other.x)
    {
        other.x = 0;
        std::cout << "ValueType(ValueType&&{" << x << "})" << std::endl;
    }
    ~ValueType() { std::cout << "~ValueType(" << x << ')'  << std::endl; }
};

template <>
struct HashFunctor<KeyType>
{
    uint64_t operator()(const KeyType& obj) const noexcept { return obj.x; }
};

int main()
{
    unordered_map<KeyType, ValueType> map1;
    map1.insert(make_pair(KeyType(1), ValueType(1)));
}

