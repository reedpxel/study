#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H

#include <vector>
#include <forward_list>

#include "hash_functor.hpp"
#include "equal_functor.hpp"
#include <iostream> // TO DO: remove

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
using std::make_pair;

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
    typename Hash = HashFunctor<Key>, 
    typename KeyEqual = EqualFunctor<Key>,
    typename Alloc = std::allocator<pair<const Key, Value>>
> class unordered_map
{
public: // TO DO: remove
    struct BaseNode
    {
        BaseNode* next;
    };

    struct Node : BaseNode
    {
        pair<const Key, Value> kv;
        // хеш хранится не по модулю длины buckets, а просто. Когда надо,
        // берется модуль от него
        uint64_t hash; // чтобы не вызывать хеш-фукнцию для каждого узла при
                       // вызове find, потому что, например, для строк это долго

//        Node(BaseNode* next, pair<const Key, Value> kv, uint64_t hash)
//                : BaseNode(next)
//                , kv(kv)
//                , hash(hash)
//        {}
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

        BaseIterator(BaseNode* ptr) : nodePtr(ptr) {}

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
            return *this;
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
        friend class unordered_map;
    public: // TO DO: set private
        BaseNode* nodePtr; 
    };

    using ArrAlloc = typename std::allocator_traits<Alloc>:: 
        template rebind_alloc<Node*>;
    using NodeAlloc = typename std::allocator_traits<Alloc>::
        template rebind_alloc<Node>;
    using ArrAllocTraits = std::allocator_traits<ArrAlloc>;
    using NodeAllocTraits = std::allocator_traits<NodeAlloc>;
public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = pair<const Key, Value>;
    using size_type = size_t;
    using difference_type = uint64_t;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using allocator_type = Alloc;
    using reference = value_type&;
    using const_reference = const reference;
    using pointer = std::allocator_traits<Alloc>::pointer;
    using const_pointer = std::allocator_traits<Alloc>::const_pointer;
    using iterator = BaseIterator<false>;
    using const_iterator = BaseIterator<true>;

    unordered_map()
            : arrAlloc()
            , nodeAlloc()
            , arrSz(0)
            , arr(nullptr)
            , sz(0)
            , maxLoadFactor(1)
            , fakeNode(nullptr)
    {
        fakeNode.next = &fakeNode;
    }

    unordered_map(unordered_map& other) // TO DO: add const
            : arrAlloc(ArrAllocTraits::propagate_on_container_copy_assignment::
                value ? other.arrAlloc : ArrAlloc())
            , nodeAlloc(NodeAllocTraits::
                propagate_on_container_copy_assignment::value ?
                other.nodeAlloc : NodeAlloc())
            , arrSz(other.arrSz)
            , arr(ArrAllocTraits::allocate(arrAlloc, arrSz))
            , sz(0)
            , maxLoadFactor(other.maxLoadFactor)
            , fakeNode(nullptr)
    {
        if (!other.sz)
        {
            fakeNode.next = &fakeNode;
            return;
        }
        BaseNode* prev = &fakeNode;
        Node* newNode = nullptr;
        try
        {
            for (const_iterator itOther = other.cbegin(); itOther != 
                other.end(); ++itOther, prev = prev->next)
            // why other.end() with const unordered_map doesn't work???
            {
                if (itOther == other.begin()) [[unlikely]]
                {
                    insertFirstNode(static_cast<Node*>(itOther.nodePtr)->kv);
                    continue;
                }
                newNode = NodeAllocTraits::allocate(nodeAlloc, 1);
                try
                {
                    NodeAllocTraits::construct(nodeAlloc, newNode, Node{
                        nullptr,static_cast<Node*>(itOther.nodePtr)->kv, 
                        static_cast<Node*>(itOther.nodePtr)->hash});
                } catch (...) {
                    NodeAllocTraits::deallocate(nodeAlloc, newNode, 1);
                    throw;
                }
                prev->next = newNode;
                if (newNode->hash % arrSz != static_cast<Node*>(prev)->hash % 
                    arrSz)
                {
                    arr[newNode->hash % arrSz] = static_cast<Node*>(prev);
                }
                ++sz;
            }
            prev->next = &fakeNode;
        } catch (...) {
            for (iterator it = begin(); ; ++it)
            {
                NodeAllocTraits::destroy(nodeAlloc, it.nodePtr);
                NodeAllocTraits::deallocate(nodeAlloc, static_cast<Node*>(
                    it.nodePtr), 1);
                if (it == iterator(prev)) break;
            }
            ArrAllocTraits::deallocate(arrAlloc, arr, arrSz);
            throw;
        }
    }

    unordered_map(unordered_map&& other)
            : arrAlloc(ArrAllocTraits::propagate_on_container_move_assignment::
                value ? std::move(other.arrAlloc) : ArrAlloc())
            , nodeAlloc(NodeAllocTraits::
                propagate_on_container_move_assignment::value ? std::move(
                other.nodeAlloc) : NodeAlloc())
            , arrSz(other.arrSz)
            , arr(other.arr)
            , sz(other.sz)
            , maxLoadFactor(other.maxLoadFactor)
            , fakeNode(other.fakeNode.next)
    {
        iterator last = other.begin();
        for (; last.nodePtr->next != &other.fakeNode; ++last);
        last.nodePtr->next = &fakeNode;
        other.arrSz = 0;
        other.arr = nullptr;
        other.sz = 0;
        other.fakeNode.next = &other.fakeNode;
    }

    std::pair<iterator, bool> insert(const value_type& value)
    {
        if (!sz)
        {
            bool arrWasAllocated = true;
            if (isNullptr(arr))
            {
                arrWasAllocated = false;
                arrSz = countArrSize(0);
                arr = ArrAllocTraits::allocate(arrAlloc, arrSz);
            }
            Node* newNode = nullptr;
            try
            {
                newNode = insertFirstNode(value);
            } catch (...) {
                if (!arrWasAllocated) 
                {
                    ArrAllocTraits::deallocate(arrAlloc, arr, arrSz);
                }
                throw;
            }
            return make_pair(iterator(newNode), true);
        }
        uint64_t hashValue = hash_(value.first);
        Node* line = arr[hashValue % arrSz];
        line = (isNullptr(line) ? 
            reinterpret_cast<Node*>(&fakeNode) : 
            static_cast<Node*>(line->next));
        bool keyFound = false;
        for (; static_cast<BaseNode*>(line) != &fakeNode && 
                line->hash % arrSz == hashValue % arrSz; 
            line = static_cast<Node*>(line->next))
        {
            if (equal_(line->kv.first, value.first))
            {
                keyFound = true;
                break;
            }
        }
        if (keyFound) return make_pair(iterator(line), false);
        if ((sz + 1) / arrSz >= maxLoadFactor) rehash(countArrSize(arrSz));
        Node* newNode = NodeAllocTraits::allocate(nodeAlloc, 1);
        try
        {
            NodeAllocTraits::construct(nodeAlloc, newNode, 
                Node{nullptr, value, hashValue});
        } catch (...) {
            NodeAllocTraits::deallocate(nodeAlloc, newNode, 1);
            throw;
        }
        insertConstructedNode(newNode, arr, arrSz);
        ++sz;
        return make_pair(iterator(newNode), true);
    }

//    std::pair<iterator, bool> insert([[maybe_unused]] value_type&& value)
//    {
//         return std::make_pair(iterator(nullptr), false);
//    }

    ~unordered_map()
    {
        ArrAllocTraits::deallocate(arrAlloc, arr, arrSz);
        if (!sz) return;
        for (Node* begin_ = static_cast<Node*>(fakeNode.next); 
            static_cast<BaseNode*>(begin_) != &fakeNode; )
        {
            Node* afterBegin = static_cast<Node*>(begin_->next);
            NodeAllocTraits::destroy(nodeAlloc, begin_);
            NodeAllocTraits::deallocate(nodeAlloc, begin_, 1);
            begin_ = afterBegin;
        }
    }

    iterator find(const Key& key_)
    {
        uint64_t hashValue = hash_(key_);
        Node* line = arr[hashValue % arrSz];
        if (isNullptr(line)) return end();
        line = static_cast<Node*>(line->next);
        bool keyFound = false;
        for (; static_cast<BaseNode*>(line) != &fakeNode && line->hash % arrSz 
                == hashValue % arrSz; line = static_cast<Node*>(line->next))
        {
            if (line->kv.first == key_)
            {
                keyFound = true;
                break;
            }
        }
        return keyFound ? iterator(line) : end();
    }

    const_iterator find(const Key& key_) const
    {
        return const_iterator(nullptr);
    }

    iterator erase(iterator pos) noexcept
    {
        BaseNode* prev = arr[static_cast<Node*>(pos.nodePtr)->hash % arrSz];
        for (; prev->next != pos.nodePtr; prev = prev->next);
        if (pos.nodePtr->next == &fakeNode || static_cast<Node*>(pos.nodePtr)->
            hash % arrSz != static_cast<Node*>(pos.nodePtr->next)->hash % 
            arrSz) // is pos.nodePtr last node in hash line
        {
            if (prev == &fakeNode || static_cast<Node*>(prev)->hash % arrSz != 
                static_cast<Node*>(pos.nodePtr)->hash % arrSz)
            // is pos.nodePtr the only node in hash line
            {
                arr[static_cast<Node*>(pos.nodePtr)->hash % arrSz] = nullptr;
                if (pos.nodePtr->next != &fakeNode)
                {
                    arr[static_cast<Node*>(pos.nodePtr->next)->hash % arrSz] = 
                        static_cast<Node*>(prev);
                }
            } else if (pos.nodePtr->next != &fakeNode) {
                arr[static_cast<Node*>(pos.nodePtr->next)->hash % arrSz] = 
                    static_cast<Node*>(prev);
            }
        }
        prev->next = pos.nodePtr->next;
        NodeAllocTraits::destroy(nodeAlloc, static_cast<Node*>(pos.nodePtr));
        NodeAllocTraits::deallocate(nodeAlloc, static_cast<Node*>(pos.nodePtr),
            1);
        --sz;
        return iterator(prev->next);
    }

    iterator erase(const_iterator pos)
    {
        return iterator(nullptr);
    }

    iterator begin() noexcept { return iterator(fakeNode.next); }
    iterator end() noexcept { return iterator(&fakeNode); }

    const_iterator begin() const noexcept 
    { 
        return const_iterator(fakeNode.next); 
    }

    const_iterator end() const noexcept 
    { 
        return const_iterator(&fakeNode); 
    }

    const_iterator cbegin() const noexcept 
    { 
        return const_iterator(fakeNode.next); 
    }

    const_iterator cend() const noexcept 
    { 
        return const_iterator(&fakeNode); 
    }

    void reserve(size_t count)
    {

    }

    void rehash(size_t newBucketCount)
    {
        if (!sz || !newBucketCount) return;
        if (sz / newBucketCount >= maxLoadFactor) return;
        Node** newArr = ArrAllocTraits::allocate(arrAlloc, newBucketCount);
        for (Node* node = static_cast<Node*>(fakeNode.next);
            static_cast<BaseNode*>(node) != &fakeNode;)
        {
            Node* nextNode = static_cast<Node*>(node->next);
            if (iterator(node) == begin()) [[unlikely]]
            {
                node->next = &fakeNode;
                newArr[node->hash % newBucketCount] = 
                    static_cast<Node*>(&fakeNode);
                fakeNode.next = node;
            } else {
                insertConstructedNode(node, newArr, newBucketCount);
            }
            node = nextNode;
        }
        ArrAllocTraits::deallocate(arrAlloc, arr, arrSz);
        arr = newArr;
        arrSz = newBucketCount;
    }

    bool empty() const noexcept { return !sz; }
    size_t size() const noexcept { return sz; }
    size_t bucket_count() const noexcept { return arrSz; }
    double load_factor() const noexcept { return sz / arrSz; } 
    double max_load_factor() const noexcept { return maxLoadFactor; } 
//    size_t bucket(const Key& key_) const noexcept
//    {
//    }

    void set_max_load_factor(size_t newMaxLoadFactor) noexcept
    {
        maxLoadFactor = newMaxLoadFactor;
    }

//    Alloc get_allocator() const noexcept { return 

    void print() noexcept
    {
        std::cout << "arrSz: " << arrSz << std::endl << "arr:\n";
        for (size_t i = 0; i < arrSz; ++i) std::cout << i << ' ' << arr[i] << 
            std::endl;
        std::cout << std::endl << "nodes:\n";
        for (iterator it = begin(); it != end(); ++it)
        {
            std::cout << it.nodePtr << ' ' << it.nodePtr->next << ' ' <<
                it->first << ' ' << it->second << ' ' << static_cast<Node*>(
                it.nodePtr)->hash % arrSz << std::endl;
        }
        std::cout << "fakeNode: " << &fakeNode << ' ' << fakeNode.next << 
            std::endl;
    };
public: // TO DO: remove
    size_t countArrSize(size_t oldArrSize) const noexcept
    {
        if (!oldArrSize) return 7;
        return 4 * oldArrSize - 1;
    }

    void insertConstructedNode(Node* constructedNode, 
        Node** arr_, size_t arrSz_) noexcept
    {
        if (isNullptr(arr_[constructedNode->hash % arrSz_]))
        {
            constructedNode->next = fakeNode.next;
            arr_[static_cast<Node*>(fakeNode.next)->hash % arrSz_] = 
                constructedNode;
            arr_[constructedNode->hash % arrSz_] = 
                static_cast<Node*>(&fakeNode);
            fakeNode.next = constructedNode;
        } else {
            constructedNode->next = 
                arr_[constructedNode->hash % arrSz_]->next;
            arr_[constructedNode->hash % arrSz_]->next = constructedNode;
        }
    }
    
    Node* insertFirstNode(const value_type& value)
    {
        Node* newNode = NodeAllocTraits::allocate(nodeAlloc, 1);
        try
        {
            NodeAllocTraits::construct(nodeAlloc, newNode, Node{&fakeNode, 
                value, hash_(value.first)});
        } catch (...) {
            NodeAllocTraits::deallocate(nodeAlloc, newNode, 1);
            throw;
        }
        fakeNode.next = newNode;
        arr[newNode->hash % arrSz] = static_cast<Node*>(&fakeNode);
        ++sz;
        return newNode;
    }

public:
    ArrAlloc arrAlloc;
    NodeAlloc nodeAlloc;
    size_t arrSz;
    Node** arr;
    size_t sz;
    double maxLoadFactor; // если load_factor() превышает это число, делается
                          // rehash
    BaseNode fakeNode;
    [[no_unique_address]] Hash hash_;
    [[no_unique_address]] KeyEqual equal_;
};

#endif
