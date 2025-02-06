#include "unordered_map.hpp"

#include <iostream>
#include <unordered_map>
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

template <>
struct std::hash<KeyType>
{
    uint64_t operator()(const KeyType& obj) const noexcept { return obj.x; }
};

template <typename T>
struct Debug
{
    Debug(T) = delete;
};

int main()
{
    unordered_map<int, std::string> map1, map2;
    map1[1] = "one";
    int two = 2;
    map1[two] = "two";
    map1.insert(make_pair(3, "three"));
    map2[1] = map1[1];
    map2[2] = map1[2];
    map2[3] = "three";
    std::cout << (map1 == map2) << std::endl;
}

