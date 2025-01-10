#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H

#include <vector>
#include <forward_list>

template <typename T>
bool isNullptr(T* ptr) 
{
#ifdef __SANITIZE_ADDRESS__
    return ptr == nullptr || ptr == (T*)0xbebebebebebebebe;
#else 
    return ptr == nullptr;
#endif
}

using std::pair;
using std::vector;
using std::forward_list;

// When unordered_map rehashes itself? 

// указатели и ссылки на эл-ты в unordered_map не инвалидируются при insert и
// erase. Итераторы инвалидируются согласно стандарту, но никто не знает почему.

// чтобы можно было сделать std::unordered_map со своим типом T, нужны:
//  - хеш-функция (либо доопределить std::hash для Т, либо написать функтор)
//  - компаратор (на равенство)

template <
    typename Key, 
    typename Value, 
    typename Hash = std::hash<Key>, 
    typename KeyEqual = std::equal_to<Key>,
    typename Allocator = std::allocator<pair<const Key, Value>>
> class unordered_map
{
    struct Node
    {
        pair<const Key, Value> kv;
        // хеш хранится не по модулю длины buckets, а просто. Когда надо,
        // берется модуль от него
        uint64_t hash; // чтобы не вызывать хеш-фукнцию для каждого узла при
                       // вызове find, потому что, например, для строк это долго
    };


    void reserve(size_t count)
    {

    }

    size_t bucket_count();

    double load_factor() const { return ???; } // отношение колва ключей к
                                               // размеру массива
    double get_max_load_factor() const { return ???; } // число, при превышении
                                                   // которого делается rehash
    void set_max_load_factor();
public:
    vector<Node*> buckets; // итератор лучше Node*
    forward_list<Node> fl;
};

#endif
