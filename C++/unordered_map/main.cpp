#include "unordered_map.hpp"

#include <iostream>
#include <unordered_map>
#include <vector>

 void* operator new(size_t count)
 {
     void* ptr = malloc(count);
     std::cout << count << " bytes allocated at " << ptr << '\n';
     return ptr;
 }
 
 void operator delete(void* ptr)
 {
     std::cout << "deallocation at " << ptr << '\n';
     free(ptr);
 }
 
 void operator delete(void* ptr, size_t count)
 {
     std::cout << count << " bytes deallocated at " << ptr << '\n';
     free(ptr);
 }

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
    std::vector<std::pair<int, std::string>> v;
    for (int i = 0; i < 10; ++i) 
    {
        v.push_back(std::make_pair(i, std::to_string(i)));
    }
    unordered_map<int, std::string> m(v.begin(), v.end());
    for (auto [x, y] : m) std::cout << x << ' ' << y << '\n';
}

//int main()
//{
//    unordered_map<int, std::string> map1;
//    unordered_map<int, std::string> map2(100);
//    std::vector<std::pair<int, std::string>> v = {
//        std::make_pair(1, "one"),
//        std::make_pair(2, "two"),
//        std::make_pair(3, "three"),
//        std::make_pair(4, "four")};
//    unordered_map<int, std::string> map3(v.begin(), v.end());
//    unordered_map<int, std::string> map4 = {
//        std::make_pair(1, "one"),
//        std::make_pair(2, "two"),
//        std::make_pair(3, "three"),
//        std::make_pair(4, "four")};
//    std::cout << "map1:\n";
//    for (auto& [x, y] : map1) std::cout << x << ' ' << y << '\n';  
//    std::cout << "map2:\n";
//    for (auto& [x, y] : map2) std::cout << x << ' ' << y << '\n';
//    std::cout << "map3:\n";
//    for (auto& [x, y] : map3) std::cout << x << ' ' << y << '\n';
//    std::cout << "map4:\n";
//    for (auto& [x, y] : map4) std::cout << x << ' ' << y << '\n';
//}
//
