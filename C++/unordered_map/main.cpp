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
    for (int j = 1; j < 1000; ++j)
    {
        unordered_map<int, std::string> map_;
        for (int i = 0; i < 1000; ++i) map_.insert(make_pair(j * i, "asdf"));
        for (int i = 0; i < 1000; ++i)
        {
            std::cout << i << ' ' << j << std::endl;
            map_.erase(map_.find(j * i));
        }
    }
}
// sizeof(std::unordered_map) == 56
