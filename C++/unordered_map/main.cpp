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

int main()
{
    unordered_map<int, std::string> map;
    for (int i = 0; i < 12; ++i) map.insert(make_pair(i, "abc"));
    unordered_map<int, std::string> map2;
    for (int i = 0; i < 12; ++i) map2.insert(make_pair(i * 113, "a"));
    map2.insert(make_pair(23, "b"));
    map2.insert(make_pair(50, "c"));
    map = std::move(map2);
    unordered_map<int, std::string> map3 = std::move(map);
    map.print();
    std::cout << "\n**********\n\n";
    map2.print();
    std::cout << "\n**********\n\n";
    map3.print();
}
// sizeof(std::unordered_map) == 56
