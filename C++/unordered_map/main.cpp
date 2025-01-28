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
    unordered_map<int, std::string> map_;
    for (int i = 0; i < 1000; ++i) 
    {
        map_.insert(make_pair(i, "adsf"));
    }
    for (int i = 0; i < 1000; ++i)
    {
        map_.erase(map_.find(i));
    }
    for (auto& pair_ : map_) std::cout << pair_.first << ' ';
    std::cout << std::endl;
    std::cout << sizeof(unordered_map<int, std::string>);
}
// sizeof(std::unordered_map) == 56
