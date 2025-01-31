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
    for (int i = 0; i < 5; ++i) map.insert(make_pair(i, ""));
    unordered_map<int, std::string> map2 = std::move(map);
    for (auto& pair_ : map) std::cout << pair_.first << ' ';
    std::cout << std::endl;
    for (auto& pair_ : map2) std::cout << pair_.first << ' ';
    std::cout << std::endl;
    map.print();
    std::cout << "\n@@@@@@@@@@@@@@@@\n\n";
    map2.print();
}
// sizeof(std::unordered_map) == 56
