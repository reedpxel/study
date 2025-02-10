#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "hash_functor.hpp"
#include "equal_functor.hpp"

#ifndef IS_NULLPTR
#define IS_NULLPTR
template <typename T>
bool isNullptr(T* ptr) 
{
#ifdef __SANITIZE_ADDRESS__
    return ptr == nullptr || ptr == (T*)0xbebebebebebebebe;
#else 
    return ptr == nullptr;
#endif
}
#endif

using std::pair;
using std::make_pair;

template <
    typename Key, 
    typename Value, 
    typename Hash = HashFunctor<Key>, 
    typename KeyEqual = EqualFunctor<Key>,
    typename Alloc = std::allocator<pair<const Key, Value>>
> class unordered_map
{
    struct BaseNode
    {
        BaseNode* next;
    };

    struct Node : BaseNode
    {
        pair<const Key, Value> kv;
        uint64_t hash;

        Node(BaseNode* next, const pair<const Key, Value>& kv, uint64_t hash)
                : BaseNode{next}
                , kv(kv)
                , hash(hash)
        {}

        Node(BaseNode* next, pair<const Key, Value>&& kv, uint64_t hash)
                : BaseNode{next}
                , kv(std::move(kv))
                , hash(hash)
        {}

        template <typename... KVArgs>
        Node(BaseNode* next, uint64_t hash, KVArgs&&... kvargs)
                : BaseNode{next}
                , kv(std::forward<KVArgs>(kvargs)...)
                , hash(hash)
        {}
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
        using node_pointer_t = std::conditional_t<IsConst, const BaseNode*,
              BaseNode*>;
        BaseIterator(node_pointer_t ptr) : nodePtr(ptr) {}

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
    private:
        node_pointer_t nodePtr; 
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
    using difference_type = int64_t;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using allocator_type = Alloc;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Alloc>::pointer;
    using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;
    using iterator = BaseIterator<false>;
    using const_iterator = BaseIterator<true>;

    unordered_map(const Alloc& alloc = Alloc())
            : arrAlloc(ArrAllocTraits::propagate_on_container_copy_assignment::
                value ? alloc : Alloc())
            , nodeAlloc(NodeAllocTraits::
                propagate_on_container_copy_assignment::value ? alloc : 
                Alloc())
            , arrSz(0)
            , arr(nullptr)
            , sz(0)
            , maxLoadFactor(1)
            , fakeNode{nullptr}
    {
        fakeNode.next = &fakeNode;
    }

    explicit unordered_map(size_t bucket_count,
                           const Hash& hash_ = Hash(),
                           const KeyEqual& keyEqual = KeyEqual(),
                           const Alloc& alloc = Alloc())
            : arrAlloc(ArrAllocTraits::propagate_on_container_copy_assignment::
                value ? alloc : Alloc())
            , nodeAlloc(NodeAllocTraits::
                propagate_on_container_copy_assignment::value ? alloc : 
                Alloc())
            , arrSz(bucket_count)
            , arr(ArrAllocTraits::allocate(arrAlloc, arrSz))
            , sz(0)
            , maxLoadFactor(1)
            , fakeNode{nullptr}
            , hash_(hash_)
            , equal_(keyEqual)
    {
        fakeNode.next = &fakeNode;
    }

    template <typename InputIt>
    unordered_map(InputIt first, InputIt last, 
                  size_t bucket_count = 7, 
                  const Hash& hash_ = Hash(),
                  const KeyEqual& keyEqual = KeyEqual(),
                  const Alloc& alloc = Alloc())
            : unordered_map(bucket_count, hash_, keyEqual, alloc)
    {
        for (InputIt it = first; it != last; ++it)
        {
            insert(*it);
        }
    }

    unordered_map(std::initializer_list<value_type> iList, 
                  size_t bucket_count = 7,
                  const Hash& hash_ = Hash(),
                  const KeyEqual& keyEqual = KeyEqual(),
                  const Alloc& alloc = Alloc())
            : unordered_map(bucket_count, hash_, keyEqual, alloc)
    {
        for (auto it = iList.begin(); it != iList.end(); ++it)
        {
            insert(*it);
        }
    }

    unordered_map(const unordered_map& other)
            : arrAlloc(ArrAllocTraits::propagate_on_container_copy_assignment::
                value ? other.arrAlloc : ArrAlloc())
            , nodeAlloc(NodeAllocTraits::
                propagate_on_container_copy_assignment::value ?
                other.nodeAlloc : NodeAlloc())
            , arrSz(other.arrSz)
            , arr(ArrAllocTraits::allocate(arrAlloc, arrSz))
            , sz(0)
            , maxLoadFactor(other.maxLoadFactor)
            , fakeNode{nullptr}
    {
        if (!other.sz)
        {
            fakeNode.next = &fakeNode;
            return;
        }
        copyNodes(other); 
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
            , fakeNode{other.fakeNode.next}
    {
        moveCtorChanges(std::forward<unordered_map>(other));
    }

    unordered_map& operator=(unordered_map& other)
    {
        if (&other == this) return *this;
        if (isNullptr(other.arr))
        {
            clearHelper();
            arrAlloc = ArrAllocTraits::
                propagate_on_container_copy_assignment::value ? 
                other.arrAlloc : ArrAlloc();
            nodeAlloc = NodeAllocTraits::
                propagate_on_container_copy_assignment::value ?
                other.nodeAlloc : NodeAlloc();
            maxLoadFactor = other.maxLoadFactor;
            return *this;
        }
        ArrAlloc oldArrAlloc = arrAlloc;
        NodeAlloc oldNodeAlloc = nodeAlloc;
        size_t oldArrSz = arrSz;
        Node** oldArr = arr;
        size_t oldSz = sz;
        double oldMaxLoadFactor = maxLoadFactor;
        BaseNode* oldFakeNodeNext = fakeNode.next;
        arrAlloc = ArrAllocTraits::
            propagate_on_container_copy_assignment::value ? 
            other.arrAlloc : ArrAlloc();
        nodeAlloc = NodeAllocTraits::
            propagate_on_container_copy_assignment::value ?
            other.nodeAlloc : NodeAlloc();
        arrSz = other.arrSz;
        sz = 0;
        maxLoadFactor = other.maxLoadFactor;
        try
        {
            arr = ArrAllocTraits::allocate(arrAlloc, arrSz);
            fakeNode.next = &fakeNode;
            copyNodes(other);
        } catch (...) {
            arrAlloc = oldArrAlloc;
            nodeAlloc = oldNodeAlloc;
            arrSz = oldArrSz;
            arr = oldArr;
            sz = oldSz;
            maxLoadFactor = oldMaxLoadFactor;
            throw;
        }
        deallocateNodesAndArr(
            oldArrAlloc,
            oldNodeAlloc,
            oldArrSz,
            oldArr,
            oldSz,
            oldFakeNodeNext);
        return *this;
    }

    unordered_map& operator=(unordered_map&& other)
    {
        if (&other == this) return *this;
        deallocateNodesAndArr();
        arrAlloc = ArrAllocTraits::
            propagate_on_container_move_assignment::value ? 
            std::move(other.arrAlloc) : ArrAlloc();
        nodeAlloc = NodeAllocTraits::
            propagate_on_container_move_assignment::value ?
            std::move(other.nodeAlloc) : NodeAlloc();
        arrSz = other.arrSz;
        arr = other.arr;
        sz = other.sz;
        maxLoadFactor = other.maxLoadFactor;
        fakeNode.next = other.fakeNode.next;
        moveCtorChanges(std::forward<unordered_map>(other));
        return *this;
    }

    // Why does not the type of the allocator matter? arrAlloc allocates Node*,
    // nodeAlloc allocates Node - neither of them allocates pair<Key, Value>. 
    // Which one must be returned?
    Alloc get_allocator() const noexcept { return arrAlloc; }

    // The following code calls one KeyType copy ctor:
    // std::pair<const KeyType, uint64_t> p(KeyType(1), 1);
    // map.insert(p);
    //
    // But if pair type is slightly different, an extra copy is made:
    // auto p = make_pair(KeyType(1), 1); or
    // auto p = make_pair(KeyType(1), 1ull); or
    // auto p = make_pair(KeyType(1), uint64_t(1); or even
    // pair<KeyType, uint64_t> p(KeyType(1), uint64_t(1)); 
    // are all not unordered_map<...>::value_type and a copy from this type to
    // value_type must be done that calls the KeyType copy ctor
    //
    // In STL unordered_map, insert does not make an extra copy in all these
    // cases. Why??????????????????????????????????????????????????????
    std::pair<iterator, bool> insert(const value_type& value)
    {
        return insertHelper(value);
    }

    std::pair<iterator, bool> insert(value_type&& value)
    {
        return insertHelper(std::move(value));
    }

    template <typename... KVArgs>
    pair<iterator, bool> emplace(KVArgs&&... kvArgs)
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
                newNode = emplaceFirstNode(std::forward<KVArgs>(kvArgs)...);
            } catch (...) {
                if (!arrWasAllocated) 
                {
                    ArrAllocTraits::deallocate(arrAlloc, arr, arrSz);
                }
                throw;
            }
            return make_pair(iterator(newNode), true);
        }
        // since key is not constructed yet, Node must be constructed first to
        // get hash and check whether an element with such key already exists 
        // or not
        Node* newNode = NodeAllocTraits::allocate(nodeAlloc, 1);
        try
        {
            NodeAllocTraits::construct(nodeAlloc, newNode, fakeNode.next, 0,
                std::forward<KVArgs>(kvArgs)...);
        } catch (...) {
            NodeAllocTraits::deallocate(nodeAlloc, newNode, 1);
            throw;
        }
        newNode->hash = hash_(newNode->kv.first);
        Node* line = arr[newNode->hash % arrSz];
        line = (isNullptr(line) ? 
            reinterpret_cast<Node*>(&fakeNode) : 
            static_cast<Node*>(line->next));
        bool keyFound = false;
        for (; static_cast<BaseNode*>(line) != &fakeNode && 
                line->hash % arrSz == newNode->hash % arrSz; 
            line = static_cast<Node*>(line->next))
        {
            if (equal_(line->kv.first, newNode->kv.first))
            {
                keyFound = true;
                break;
            }
        }
        if (keyFound) 
        {
            NodeAllocTraits::destroy(nodeAlloc, newNode);
            NodeAllocTraits::deallocate(nodeAlloc, newNode, 1);
            return make_pair(iterator(line), false);
        }
        if ((sz + 1) / arrSz >= maxLoadFactor) rehash(countArrSize(arrSz));
        insertConstructedNode(newNode, arr, arrSz);
        ++sz;
        return make_pair(iterator(newNode), true);
    }

    ~unordered_map() { deallocateNodesAndArr(); }

    iterator find(const Key& key_)
    {
        return iterator(const_cast<BaseNode*>(findHelper(key_)));
    }

    const_iterator find(const Key& key_) const
    {
        return const_iterator(findHelper(key_));
    }

    iterator erase(iterator pos) noexcept
    {
        BaseNode* prev = arr[static_cast<Node*>(pos.nodePtr)->hash % arrSz];
        for (; prev->next != pos.nodePtr; prev = prev->next);
        if (pos.nodePtr->next == &fakeNode || static_cast<Node*>(pos.nodePtr)->
            hash % arrSz != static_cast<Node*>(pos.nodePtr->next)->hash % 
            arrSz) // is pos.nodePtr the last node in hash line
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

    iterator erase(const_iterator pos) noexcept
    {
        return erase(iterator(const_cast<BaseNode*>(pos.nodePtr)));
    }

    Value& at(const Key& key_)
    {
        iterator it = find(key_);
        if (it == end()) 
        {
            throw std::out_of_range("Element with such key is missing");
        }
        return (*it).second;
    }

    const Value& at(const Key& key_) const
    {
        iterator it = find(key_);
        if (it == end()) 
        {
            throw std::out_of_range("Element with such key is missing");
        }
        return (*it).second;
    }

    Value& operator[](const Key& key_)
    {
        iterator it = find(key_);
        if (it == end())
        {
            auto res = emplace(key_, Value());
            return (*res.first).second;
        }
        return (*it).second;
    }
    
    Value& operator[](Key&& key_)
    {
        iterator it = find(key_);
        if (it == end())
        {
            auto res = emplace(key_, Value());
            return (*res.first).second;
        }
        return (*it).second;
    }

    size_t count(const Key& key_) const
    {
        return find(key_) == end() ? 0 : 1;
    }

    size_t bucket(const Key& key_) const
    {
        if (isNullptr(arr) || !arrSz) return 0;
        return hash_(key_) % arrSz;
    }

    size_t bucket_size(size_t n) const
    {
        if (isNullptr(arr) || n >= arrSz || isNullptr(arr[n])) return 0;
        size_t res = 0;
        for (BaseNode* node = arr[n]->next; node != &fakeNode &&
            static_cast<Node*>(node)->hash % arrSz == n; node = node->next, 
            ++res);
        return res;
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
        rehash(std::ceil(count / maxLoadFactor));
    }

    void rehash(size_t newBucketCount)
    {
        if (isNullptr(arr))
        {
            if (newBucketCount)
            {
                arr = ArrAllocTraits::allocate(arrAlloc, newBucketCount);
                arrSz = newBucketCount;
            }
            return;
        }
        if (!newBucketCount || sz / newBucketCount >= maxLoadFactor)
        {
            return;
        }
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

    void set_max_load_factor(double newMaxLoadFactor) noexcept
    {
        maxLoadFactor = newMaxLoadFactor;
    }

    void clear() noexcept { clearHelper(); }
    Hash hash_function() const { return hash_; }
    KeyEqual key_eq() const { return equal_; }

    bool operator==(unordered_map& other)
    {
        if (sz != other.sz) return false;
        return std::is_permutation(begin(), end(), other.begin(), 
            [this](const pair<Key, Value>& thisPair, const pair<Key, Value>& 
                otherPair)
            {
                return equal_(thisPair.first, otherPair.first) &&
                    (thisPair.second == otherPair.second);
            }); 
    }

    void swap(unordered_map& other)
    {
        if (ArrAllocTraits::propagate_on_container_swap::value)
        {
            std::swap(arrAlloc, other.arrAlloc);
            std::swap(nodeAlloc, other.nodeAlloc);
        }
        BaseNode* thisLast = fakeNode.next;
        BaseNode* otherLast = other.fakeNode.next;
        for (; thisLast->next != &fakeNode; thisLast = thisLast->next);
        for (; otherLast->next != &other.fakeNode; otherLast = 
            otherLast->next);
        std::swap(thisLast->next, otherLast->next);
        std::swap(arrSz, other.arrSz);
        std::swap(arr, other.arr);
        std::swap(sz, other.sz);
        std::swap(maxLoadFactor, other.maxLoadFactor);
        std::swap(fakeNode, other.fakeNode);
    }
private:
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

    template <typename ValueType>
    pair<iterator, bool> insertHelper(ValueType&& value)
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
                newNode = insertFirstNode(std::forward<ValueType>(value));
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
            uint64_t hashValue = hash_(value.first);
            // If value is lvalue, Node(BaseNode*, const pair&, hash) will be
            // called. If value is rvalue, Node(BaseNode*, pair&&, hash) will 
            // be called.
            NodeAllocTraits::construct(nodeAlloc, newNode, nullptr, 
                std::forward<ValueType>(value), hashValue);
        } catch (...) {
            NodeAllocTraits::deallocate(nodeAlloc, newNode, 1);
            throw;
        }
        insertConstructedNode(newNode, arr, arrSz);
        ++sz;
        return make_pair(iterator(newNode), true);
    }

    template <typename ValueType>
    Node* insertFirstNode(ValueType&& value)
    {
        Node* newNode = NodeAllocTraits::allocate(nodeAlloc, 1);
        try
        {
            uint64_t hashValue = hash_(value.first);
            NodeAllocTraits::construct(nodeAlloc, newNode, &fakeNode, 
                std::forward<ValueType>(value), hashValue);
        } catch (...) {
            NodeAllocTraits::deallocate(nodeAlloc, newNode, 1);
            throw;
        }
        fakeNode.next = newNode;
        arr[newNode->hash % arrSz] = static_cast<Node*>(&fakeNode);
        ++sz;
        return newNode;
    }

    template <typename... KVArgs>
    Node* emplaceFirstNode(KVArgs&&... kvargs)
    // kvargs are the arguments of constructors of key and value, not all the
    // fields of Node (next and hash must not be among kvargs)
    {
        Node* newNode = NodeAllocTraits::allocate(nodeAlloc, 1);
        try
        {
            NodeAllocTraits::construct(nodeAlloc, newNode, &fakeNode, 0, 
                std::forward<KVArgs>(kvargs)...); 
            newNode->hash = hash_(newNode->kv.first);
        } catch (...) {
            NodeAllocTraits::deallocate(nodeAlloc, newNode, 1);
            throw;
        }
        fakeNode.next = newNode;
        arr[newNode->hash % arrSz] = static_cast<Node*>(&fakeNode);
        ++sz;
        return newNode;
    }

    void deallocateNodesAndArr(
        ArrAlloc arrAlloc_, 
        NodeAlloc nodeAlloc_,
        size_t arrSz_, 
        Node** arr_,
        size_t sz_,
        BaseNode* fakeNodeNext_) noexcept
    {
        if (isNullptr(arr_)) return;
        ArrAllocTraits::deallocate(arrAlloc_, arr_, arrSz_);
        if (!sz_) return;
        for (Node* begin_ = static_cast<Node*>(fakeNodeNext_); 
            static_cast<BaseNode*>(begin_) != &fakeNode; )
        {
            Node* afterBegin = static_cast<Node*>(begin_->next);
            NodeAllocTraits::destroy(nodeAlloc_, begin_);
            NodeAllocTraits::deallocate(nodeAlloc_, begin_, 1);
            begin_ = afterBegin;
        }
    }

    void deallocateNodesAndArr() noexcept
    {
        deallocateNodesAndArr(
            arrAlloc,
            nodeAlloc,
            arrSz,
            arr,
            sz,
            fakeNode.next);
    }

    void clearHelper() noexcept
    {
        deallocateNodesAndArr();
        arrSz = 0;
        arr = nullptr;
        sz = 0;
        fakeNode.next = &fakeNode;
    }

    void copyNodes(const unordered_map& other)
    {
        BaseNode* prev = &fakeNode;
        Node* newNode = nullptr;
        try
        {
            for (const_iterator itOther = other.cbegin(); itOther != 
                other.cend(); ++itOther, prev = prev->next)
            {
                if (itOther == other.begin()) [[unlikely]]
                {
                    insertFirstNode(static_cast<const Node*>(itOther.nodePtr)->
                        kv);
                    continue;
                }
                newNode = NodeAllocTraits::allocate(nodeAlloc, 1);
                try
                {
                    NodeAllocTraits::construct(nodeAlloc, newNode, Node{
                        nullptr, static_cast<const Node*>(itOther.nodePtr)->kv, 
                        static_cast<const Node*>(itOther.nodePtr)->hash});
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

    template <typename T>
    void moveCtorChanges(T&& other) noexcept
    {
        iterator last = other.begin();
        for (; last.nodePtr->next != &other.fakeNode; ++last);
        last.nodePtr->next = &fakeNode;
        other.arrSz = other.sz = 0;
        other.arr = nullptr;
        other.fakeNode.next = &other.fakeNode;
    }

    const BaseNode* findHelper(const Key& key_) const
    {
        if (isNullptr(arr)) return &fakeNode;
        uint64_t hashValue = hash_(key_);
        const Node* line = arr[hashValue % arrSz];
        if (isNullptr(line)) return &fakeNode;
        line = static_cast<const Node*>(line->next);
        bool keyFound = false;
        for (; static_cast<const BaseNode*>(line) != &fakeNode && line->hash % 
            arrSz == hashValue % arrSz; line = static_cast<const Node*>(line->
            next))
        {
            if (line->kv.first == key_)
            {
                keyFound = true;
                break;
            }
        }
        return keyFound ? line : &fakeNode;
    }
private:    
    ArrAlloc arrAlloc;
    NodeAlloc nodeAlloc;
    size_t arrSz;
    Node** arr;
    size_t sz;
    double maxLoadFactor;
    BaseNode fakeNode;
    [[no_unique_address]] Hash hash_;
    [[no_unique_address]] KeyEqual equal_;
};

#endif
