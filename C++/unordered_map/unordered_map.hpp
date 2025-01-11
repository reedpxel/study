#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H

#include <vector>
#include <forward_list>

// TO DO: set public to private/protected where needed

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

// как определять новый размер arr при перехешировании? Размер должен (по идее,
// но не обязательно) быть простым числом в несколько раз больше текущего
// размера

template <
    typename Key, 
    typename Value, 
    typename Hash = std::hash<Key>, 
    typename KeyEqual = std::equal_to<Key>,
    typename Alloc = std::allocator<pair<const Key, Value>>
> class unordered_map
{
    struct BaseNode
    {
        Node* next;
    };

    struct Node : BaseNode
    {
        pair<const Key, Value> kv;
        // хеш хранится не по модулю длины buckets, а просто. Когда надо,
        // берется модуль от него
        uint64_t hash; // чтобы не вызывать хеш-фукнцию для каждого узла при
                       // вызове find, потому что, например, для строк это долго
    };

    template <bool IsConst>
    class BaseIterator
    {
    public:
        using difference_type = size_t;
        using value_type = std::conditional_t<
            IsConst, const pair<const Key, Value>, pair<const Key, Value>>;
        using pointer = std::conditional_t<
            IsConst, const pair<const Key, Value>*, pair<const Key, Value>*>;
        using reference = std::conditional_t<
            IsConst, const pair<const Key, Value>&, pair<const Key, Value>&>;
        using iterator_category = std::forward_iterator_tag;

        reference operator*() const noexcept
        {
            // if nodePtr points at a Node, static_cast BaseNode->Node is ok,
            // if nodePtr points at the fakeNode, the iterator is an end()
            // iterator, dereferencing it is UB as well as the cast
            return static_cast<Node*>(nodePtr)->kv;
        }

        pointer operator->() const noexcept
        {
            return &(static_cast<Node*>(nodePtr)->kv);
        }

        BaseIterator& operator++() noexcept
        {
            nodePtr = nodePtr->next;
        }

        BaseIterator operator++(int) noexcept
        {
            BaseIterator res = *this;
            nodePtr = nodePtr->next;
            return res;
        }

        bool operator==(const BaseIterator& other) const noexcept
        {
            return nodePtr == other.nodePtr;
        }

        bool operator!=(const BaseIterator& other) const noexcept
        {
            return !(*this == other);
        }
        
        operator BaseIterator<true>() const noexcept
        {
            return BaseIterator<true>(nodePtr);
        }
    private:
        BaseNode* nodePtr; 
    };

    using iterator = BaseIterator<false>;
    using const_iterator = BaseIterator<true>;
    using ArrAlloc = typename std::allocator_traits<Alloc>:: 
        template rebind_alloc<Node*>;
    using NodeAlloc = typename std::allocator_traits<Alloc>::
        template rebind_alloc<Node>;
    using ArrAllocTraits = std::allocator_traits<ArrAlloc>;
    using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

    void reserve(size_t count)
    {

    }

//    size_t bucket_count();
//
//    double load_factor() const { return ???; } // отношение колва ключей к
//                                               // размеру массива
//    double get_max_load_factor() const { return ???; } // число, при превышении
//                                                   // которого делается rehash
//    void set_max_load_factor();
public:
    Node** arr;
    size_t arrSz;
    size_t sz;
    BaseNode fakeNode;
    ArrAlloc arrAlloc;
    NodeAlloc nodeAlloc;
};

#endif
