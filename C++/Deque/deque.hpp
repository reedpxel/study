#ifndef DEQUE_H
#define DEQUE_H
#include <memory>
#include <cstdint>
#include <exception>
#include <iterator>
#include <cmath>
#define ELEMENTS_IN_BUCKET 32ull
#define BUCKET_RESERVE 3ull

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

template <typename T, typename Alloc = std::allocator<T>>
class deque
{
protected:
    template <bool IsConst>
    class BaseIterator
    {
        friend class deque;
    public:
        using difference_type = int64_t;
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;
        using iterator_category = std::random_access_iterator_tag;
    protected:
        BaseIterator(T** innerPtr = nullptr, T* outerPtr = nullptr) noexcept
            : innerPtr(innerPtr)
            , outerPtr(outerPtr)
        {}
    public:
        reference operator*() const noexcept { return *outerPtr; }
        pointer operator->() const noexcept { return outerPtr; }

        // a check that iterators are on the same container is not done
        std::strong_ordering
            operator<=>(const BaseIterator& other) const noexcept
        {
            return innerPtr == other.innerPtr ?
                outerPtr <=> other.outerPtr :
                innerPtr <=> other.innerPtr;
        }

        bool operator==(const BaseIterator& other) const noexcept
        {
            return innerPtr == other.innerPtr && outerPtr == other.outerPtr ||
               isNullptr(*innerPtr); 
        }

        BaseIterator& operator++() noexcept
        {
            if (outerPtr - *innerPtr >= ELEMENTS_IN_BUCKET - 1)
            {
                ++innerPtr;
                if (!isNullptr(*innerPtr)) [[likely]]
                {
                    outerPtr = *innerPtr;
                } else {
                    outerPtr = nullptr;
                }
            } else {
                ++outerPtr;
            }
            return *this;
        }

        BaseIterator operator++(int) noexcept
        {
            BaseIterator res = *this;
            ++(*this);
            return res;
        }

        BaseIterator& operator--() noexcept
        {
            if (outerPtr == *innerPtr)
            {
                --innerPtr;
                outerPtr = *innerPtr + (ELEMENTS_IN_BUCKET - 1);
            } else {
                --outerPtr;
            }
            return *this;
        }

        BaseIterator operator--(int) noexcept
        {
            BaseIterator res = *this;
            --(*this);
            return res;
        }

        BaseIterator& operator+=(int64_t num) noexcept
        {
            int64_t numWithShift = num + (outerPtr - *innerPtr);
            innerPtr += numWithShift / ELEMENTS_IN_BUCKET;
            outerPtr = *innerPtr + numWithShift % ELEMENTS_IN_BUCKET;
            return *this;
        }

        BaseIterator operator+(int64_t num) const noexcept
        {
            BaseIterator res = *this;
            res += num;
            return res;
        }

        BaseIterator& operator-=(int64_t num) noexcept
        {
            int64_t outerPtrShift = (outerPtr - *innerPtr) - num;
            if (outerPtrShift < 0)
            {
                int64_t innerPtrShift = (ELEMENTS_IN_BUCKET - 1 - 
                    outerPtrShift) / ELEMENTS_IN_BUCKET;
                innerPtr -= innerPtrShift;
                outerPtr = *innerPtr + innerPtrShift * ELEMENTS_IN_BUCKET +
                    outerPtrShift;
            } else {
                outerPtr -= num;
            }
            return *this;
        }

        BaseIterator operator-(int64_t num) const noexcept
        {
            BaseIterator res = *this;
            res -= num;
            return res;
        }

        difference_type operator-(const BaseIterator& other) const noexcept
        {
            difference_type res = ELEMENTS_IN_BUCKET * 
                (innerPtr - other.innerPtr);
            difference_type thisShift = isNullptr(outerPtr) ? 0 : 
                outerPtr - *innerPtr;
            difference_type otherShift = isNullptr(outerPtr) ? 0 : 
                other.outerPtr - *other.innerPtr;
            return res + thisShift - otherShift;
        }

        operator BaseIterator<true>() const noexcept
        {
            return BaseIterator<true>(innerPtr, outerPtr);
        }
    protected:
        T** innerPtr;
        T* outerPtr;
    };
public:
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = int64_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::allocator_traits<Alloc>::pointer;
    using const_pointer = std::allocator_traits<Alloc>::const_pointer;
    using iterator = BaseIterator<false>;
    using const_iterator = BaseIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
protected: 
    using InnerAlloc = 
        typename std::allocator_traits<Alloc>::template rebind_alloc<T*>;
    using AllocTraits = std::allocator_traits<Alloc>;
    using InnerAllocTraits = std::allocator_traits<InnerAlloc>;
public:
    explicit deque(const Alloc& alloc = Alloc()) noexcept
            : alloc(alloc)
            , innerAlloc(alloc)
            , sz(0)
            , bucketCount(0)
            , innerBuffer(nullptr)
            , itBegin()
            , itEnd()
    {}

    explicit deque(size_t count, const T& value,
        const Alloc& alloc = Alloc())
            : alloc(alloc)
            , innerAlloc(alloc)
            , sz(count)
            , bucketCount(count ?
                2 * BUCKET_RESERVE + 1 + (count + ELEMENTS_IN_BUCKET - 1) / 
                    ELEMENTS_IN_BUCKET : 
                0)
            , innerBuffer(count ? // exc tested
                InnerAllocTraits::allocate(innerAlloc, bucketCount) : nullptr)
            , itBegin() // initialized in body, because buckets are not 
            , itEnd()   // allocated
    {
        initializeElements(value);
    }

    explicit deque(size_t count, const Alloc& alloc = Alloc())
            : deque(count, T(), alloc)
    {}

    deque(std::initializer_list<T> init, const Alloc& alloc = Alloc())
            : alloc(alloc)
            , innerAlloc(alloc)
            , sz(init.size())
            , bucketCount(sz ? 
                (sz + ELEMENTS_IN_BUCKET - 1) / ELEMENTS_IN_BUCKET + 
                    2 * BUCKET_RESERVE + 1: 0)
            , innerBuffer(sz ? 
                InnerAllocTraits::allocate(innerAlloc, bucketCount) : nullptr)
            , itBegin()
            , itEnd()
    {
        initializeElements(init);
    }

    // TO DO: deque<int> d(10, 1) is considered to call this ctor, even though
    // ints are not iterators. How can deque(count, value) be called instead?
    template <typename InputIt>
    deque(InputIt first, InputIt last, const Alloc& alloc = Alloc()) 
            : deque(alloc) // the delegating ctor call ends at this point of 
    // program, that is why a dtor will be called for an object that will throw
    // an exception in curly brackets of this ctor. 
    {
        size_t elementsCreated = 0;
        try
        {
            for (InputIt it = first; it != last; ++it, ++elementsCreated)
            {
                push_back(*it);
            }
        } catch (...) {
            for (size_t i = 0; i < elementsCreated; ++i) pop_back();
        }
    }

    deque(const deque& other, const Alloc& alloc_)
            : alloc(alloc_)
            , innerAlloc(alloc_)
            , sz(other.sz)
            , bucketCount(other.bucketCount)
            , innerBuffer(sz ? // exc tested 
                InnerAllocTraits::allocate(innerAlloc, bucketCount) : nullptr)
            , itBegin()
            , itEnd()
    {
        if (sz) copyBuffer(other);
    }

    deque(const deque& other) noexcept
            : deque(other, 
                AllocTraits::propagate_on_container_copy_assignment::value ?
                other.alloc :
                Alloc()) 
    {}

    deque(deque&& other, const Alloc& alloc_) noexcept
            : alloc(alloc_)
            , innerAlloc(alloc_)
            , sz(other.sz)
            , bucketCount(other.bucketCount)
            , innerBuffer(other.innerBuffer)
            , itBegin(other.itBegin)
            , itEnd(other.itEnd)
    {
        other.sz = 0;
        other.bucketCount = 0;
        other.innerBuffer = nullptr;
        other.itBegin = other.itEnd = iterator();
    }

    deque(deque&& other) noexcept
            : deque(std::move(other), AllocTraits::
                propagate_on_container_move_assignment::value ? 
                std::move(other.alloc) : Alloc())
    {}

    ~deque() { clearHelper(); }

    deque& operator=(const deque& other)
    {
        if (this == &other) return *this;
        if (!other.sz)
        {
            clearHelper();
            sz = 0;
            bucketCount = 0;
            innerBuffer = nullptr;
            itBegin = iterator(nullptr, nullptr);
            itEnd = itBegin;
            return *this;
        }
        // Memorizing old fields
        Alloc oldAlloc = alloc;
        InnerAlloc oldInnerAlloc = innerAlloc;
        if constexpr (!AllocTraits::is_always_equal::value)
        {
            alloc = 
                AllocTraits::propagate_on_container_copy_assignment::value ?
                    other.alloc : Alloc();
            innerAlloc = 
                InnerAllocTraits::propagate_on_container_copy_assignment::value 
                    ? other.innerAlloc : InnerAlloc();
        }
        size_t oldSz = sz;
        size_t oldBucketCount = bucketCount;
        T** oldInnerBuffer = innerBuffer;
        iterator oldItBegin = itBegin;
        iterator oldItEnd = itEnd;
        // Allocating new innerBuffer
        innerBuffer = InnerAllocTraits::allocate(
            innerAlloc, other.bucketCount);
        // Copying data buffers and fields
        sz = other.sz;
        bucketCount = other.bucketCount;
        try
        {
            copyBuffer(other);
        } catch (...) { // exc tested
            sz = oldSz;
            bucketCount = oldBucketCount;
            innerBuffer = oldInnerBuffer;
            itBegin = oldItBegin;
            alloc = oldAlloc; // according to the standard, allocators do not 
            innerAlloc = oldInnerAlloc; // throw in copy ctors and operator=
            throw;
        }
        clearHelper(oldAlloc,
                    oldInnerAlloc,
                    oldBucketCount,
                    oldInnerBuffer,
                    oldItBegin,
                    oldItEnd);
        return *this;
    }

    deque& operator=(deque&& other) noexcept
    {
        if (this == &other) return *this;
        clear();
        if (AllocTraits::propagate_on_container_move_assignment::value)
        {
            alloc = std::move(other.alloc);
            innerAlloc = std::move(other.innerAlloc);
        }
        sz = other.sz;
        bucketCount = other.bucketCount;
        innerBuffer = other.innerBuffer;
        itBegin = other.itBegin;
        itEnd = other.itEnd;
        other.sz = other.bucketCount = 0;
        other.innerBuffer = nullptr;
        other.itBegin = other.itEnd = iterator();
        return *this;
    }

    iterator begin() noexcept { return itBegin; }
    iterator end() noexcept { return itEnd; }
    const_iterator begin() const noexcept { return itBegin; }
    const_iterator end() const noexcept { return itEnd; }
    const_iterator cbegin() const noexcept { return itBegin; }
    const_iterator cend() const noexcept { return itEnd; }
    reverse_iterator rbegin() noexcept { return reverse_iterator(itEnd); }
    reverse_iterator rend() noexcept { return reverse_iterator(itBegin); }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(itEnd);
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(itBegin);
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(itEnd);
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(itBegin);
    }

    T& operator[](size_t index) noexcept { return *(itBegin + index); }

    const T& operator[](size_t index) const noexcept 
    { 
        return *(itBegin + index); 
    }

    T& at(size_t index)
    {
        if (index >= sz) 
        {
            throw std::out_of_range("Out of range\n");
        }
        return (*this)[index];
    }

    const T& at(size_t index) const
    {
        if (index >= sz) 
        {
            throw std::out_of_range("Out of range\n");
        }
        return (*this)[index];
    }

    // calling front() and back() on an empty container is UB, this case is not
    // handled
    T& front() noexcept { return *itBegin; }
    T& back() noexcept { return *(itEnd - 1); }
    const T& front() const noexcept { return *const_iterator(itBegin); }
    const T& back() const noexcept { return *const_iterator(itEnd - 1); }
    bool empty() const noexcept { return !sz; }
    size_t size() const noexcept { return sz; }

    std::weak_ordering operator<=>(const deque& other) const noexcept
    {
        return sz <=> other.sz;
    }

    bool operator==(const deque& other) const noexcept
    {
        if (sz != other.sz) return false;
        for (auto it = begin(), it_other = other.begin(); it != end(); 
            ++it, ++it_other)
        {
            if (*it != *it_other) return false;
        }
        return true;
    }

    Alloc get_allocator() const noexcept { return alloc; }
    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(std::move(value)); }

    template <typename... Args>
    reference emplace_back(Args&&... args)
    {
        if (!innerBuffer) [[unlikely]]
        {
            return pushInEmptyContainer(std::forward<Args>(args)...);
        }
        if (itEnd.innerPtr - innerBuffer == bucketCount - 1) [[unlikely]]
        {
            T** newInnerBuffer = InnerAllocTraits::allocate(innerAlloc,
                itEnd.innerPtr - itBegin.innerPtr + bucketCount);
            size_t newBucketCount = 
                itEnd.innerPtr - itBegin.innerPtr + bucketCount;
            for (size_t i = itBegin.innerPtr - innerBuffer;
                i <= itEnd.innerPtr - innerBuffer; ++i)
            {
                newInnerBuffer[i] = innerBuffer[i];
            }
            try
            {
                newInnerBuffer[bucketCount - 1] = AllocTraits::allocate(
                    alloc, ELEMENTS_IN_BUCKET);
            } catch (...) { // exc tested
                InnerAllocTraits::deallocate(innerAlloc, newInnerBuffer,
                    itEnd.innerPtr - itBegin.innerPtr + bucketCount);
                throw;
            }
            try
            {
                AllocTraits::construct(alloc, newInnerBuffer[bucketCount - 1],
                    std::forward<Args>(args)...);
            } catch (...) { // exc tested
                AllocTraits::deallocate(alloc, newInnerBuffer[bucketCount - 1],
                    ELEMENTS_IN_BUCKET);
                InnerAllocTraits::deallocate(innerAlloc, newInnerBuffer,
                    itEnd.innerPtr - itBegin.innerPtr + bucketCount);
                throw;
            }
            InnerAllocTraits::deallocate(innerAlloc, innerBuffer, bucketCount);
            ++sz;
            itEnd = iterator(newInnerBuffer + bucketCount - 1,
                newInnerBuffer[bucketCount - 1] + 1);
            size_t oldBucketCount = bucketCount;
            bucketCount = newBucketCount;
            itBegin.innerPtr =
                newInnerBuffer + (itBegin.innerPtr - innerBuffer);
            innerBuffer = newInnerBuffer;
            return innerBuffer[oldBucketCount - 1][0];
        } 
        if (!bucketAllocated(itEnd.innerPtr - innerBuffer)) [[unlikely]]
        {
            innerBuffer[itEnd.innerPtr - innerBuffer] =
                AllocTraits::allocate(alloc, ELEMENTS_IN_BUCKET);
            try
            {
                itEnd.outerPtr = *itEnd.innerPtr;
                AllocTraits::construct(alloc, itEnd.outerPtr, 
                    std::forward<Args>(args)...);
                ++sz;
                ++itEnd.outerPtr;
            } catch (...) { // exc tested
                AllocTraits::deallocate(
                    alloc, 
                    innerBuffer[itEnd.innerPtr - innerBuffer],
                    ELEMENTS_IN_BUCKET);
                innerBuffer[itEnd.innerPtr - innerBuffer] = nullptr;
                itEnd.outerPtr = nullptr;
                throw;
            }
            return *(itEnd.outerPtr - 1);
        }
        AllocTraits::construct(alloc, itEnd.outerPtr, 
            std::forward<Args>(args)...);
        ++itEnd;
        ++sz;
        return *(itEnd.outerPtr - 1);
    }

    // calling pop_back on an empty container is UB, end() iterator invalidates
    void pop_back() noexcept
    {
        --itEnd;
        AllocTraits::destroy(alloc, itEnd.outerPtr);
        --sz;
    }

    void push_front(const T& value) { emplace_front(value); }
    void push_front(T&& value) { emplace_front(std::move(value)); }

    template <typename... Args>
    reference emplace_front(Args&&... args)
    {
        if (!innerBuffer) [[unlikely]]
        {
            return pushInEmptyContainer(std::forward<Args>(args)...);
        }
        if (itBegin.innerPtr == innerBuffer &&
            itBegin.outerPtr == *innerBuffer) [[unlikely]]
        {
            size_t bucketsAllocated = itEnd.innerPtr - itBegin.innerPtr + 1;
            T** newInnerBuffer = InnerAllocTraits::allocate(innerAlloc,
                bucketsAllocated + bucketCount);
            for (size_t i = 0; i <= itEnd.innerPtr - innerBuffer; ++i)
            {
                newInnerBuffer[bucketsAllocated + i] = innerBuffer[i];
            }
            try
            {
                newInnerBuffer[bucketsAllocated - 1] = AllocTraits::allocate(
                    alloc, ELEMENTS_IN_BUCKET);
            } catch (...) { // exc tested
                InnerAllocTraits::deallocate(innerAlloc, newInnerBuffer, 
                    bucketsAllocated + bucketCount);
                throw;
            }
            itBegin.innerPtr = newInnerBuffer + bucketsAllocated - 1;
            itBegin.outerPtr = newInnerBuffer[bucketsAllocated - 1] +
                ELEMENTS_IN_BUCKET - 1;
            try
            {
                AllocTraits::construct(alloc, itBegin.outerPtr, 
                    std::forward<Args>(args)...);
            } catch (...) { // exc tested
                AllocTraits::deallocate(alloc, 
                    newInnerBuffer[bucketsAllocated - 1], ELEMENTS_IN_BUCKET);
                InnerAllocTraits::deallocate(innerAlloc, newInnerBuffer,
                    bucketsAllocated + bucketCount);
                itBegin.innerPtr = innerBuffer;
                itBegin.outerPtr = *innerBuffer;
                throw;
            }
            ++sz;
            bucketCount += bucketsAllocated;
            itEnd.innerPtr = newInnerBuffer + 2 * bucketsAllocated - 1;
            InnerAllocTraits::deallocate(innerAlloc, innerBuffer, bucketCount);
            innerBuffer = newInnerBuffer;
            return *itBegin;
        }
        if (*itBegin.innerPtr == itBegin.outerPtr &&
            !bucketAllocated(itBegin.innerPtr - innerBuffer - 1)) [[unlikely]]
        {
            innerBuffer[itBegin.innerPtr - innerBuffer - 1] =
                AllocTraits::allocate(alloc, ELEMENTS_IN_BUCKET);
            try
            {
                --itBegin;
                AllocTraits::construct(alloc, itBegin.outerPtr, 
                    std::forward<Args>(args)...);
                ++sz;
            } catch (...) { // exc tested
                ++itBegin;
                AllocTraits::deallocate(alloc, 
                    innerBuffer[itBegin.innerPtr - innerBuffer - 1], 
                    ELEMENTS_IN_BUCKET);
                innerBuffer[itBegin.innerPtr - innerBuffer - 1] = nullptr;
                throw;
            }
            return *itBegin;
        }
        --itBegin;
        try
        {
            AllocTraits::construct(alloc, itBegin.outerPtr, 
                std::forward<Args>(args)...);
        } catch (...) { // exc tested
            ++itBegin;
            throw;
        }
        ++sz;
        return *itBegin;
    }

    void pop_front() noexcept
    {
        AllocTraits::destroy(alloc, itBegin.outerPtr);
        ++itBegin;
        --sz;
    }

    void clear() noexcept
    {
        clearHelper();
        sz = 0;
        bucketCount = 0;
        innerBuffer = nullptr;
        itBegin = itEnd = iterator();
    }

    // the insert method gives strong exception guarantee if an exception is
    // thrown by any method except of:
    //      - T(const T&)
    //      - T(T&&)
    //      - operator=(const T&)
    //      - operator=(T&&)
    //      - any iterator methods
    // UPD: the method gives strong exception guarantee, if T(const T&) is
    // noexcept. If it is true and T(T&&) is also noexcept, move ctor is used 
    // to move objects. Otherwise, objects are moved, but the method does not 
    // give strong exception guarantee
    iterator insert(iterator pos, const T& value = T())
    {
        if (!innerBuffer) [[unlikely]]
        {
            pushInEmptyContainer(value);
            return itBegin;
        }
        if (itEnd.innerPtr - innerBuffer == bucketCount - 1) [[unlikely]]
        {
            size_t newBucketCount = 
                itEnd.innerPtr - itBegin.innerPtr + bucketCount;
            T** newInnerBuffer = InnerAllocTraits::allocate(innerAlloc,
                newBucketCount);
            for (size_t i = itBegin.innerPtr - innerBuffer;
                i <= itEnd.innerPtr - innerBuffer; ++i)
            {
                newInnerBuffer[i] = innerBuffer[i];
            }
            try
            {
                newInnerBuffer[bucketCount - 1] =
                    AllocTraits::allocate(alloc, ELEMENTS_IN_BUCKET);
            } catch (...) { // exc tested
                InnerAllocTraits::deallocate(innerAlloc, newInnerBuffer,
                    newBucketCount);
                throw;
            }
            pos.innerPtr = newInnerBuffer + (pos.innerPtr - innerBuffer);
            InnerAllocTraits::deallocate(innerAlloc, innerBuffer, bucketCount);
            itBegin.innerPtr = newInnerBuffer +
                (itBegin.innerPtr - innerBuffer);
            itEnd.innerPtr = newInnerBuffer + (itEnd.innerPtr - innerBuffer);
            itEnd.outerPtr = *itEnd.innerPtr;
            innerBuffer = newInnerBuffer;
            bucketCount = newBucketCount;
        }
        if (!bucketAllocated(itEnd.innerPtr - innerBuffer)) [[unlikely]]
        {
            innerBuffer[itEnd.innerPtr - innerBuffer] = 
                AllocTraits::allocate(alloc, ELEMENTS_IN_BUCKET);
            itEnd.outerPtr = *itEnd.innerPtr;
        }
        if (!pos.outerPtr) pos.outerPtr = itEnd.outerPtr;
        for (iterator it = itEnd; it != pos; --it)
        {
            if constexpr ((noexcept(T(std::declval<T>())) || 
                noexcept(T(std::declval<const T>()))) && 
                !noexcept(T(std::move(std::declval<T>()))))
            {
                // exception safe copying
                AllocTraits::construct(alloc, it.outerPtr, *(it - 1));
            } else {
                // calling move ctor, which may or may not be noexcept
                AllocTraits::construct(alloc, it.outerPtr, 
                    std::move(*(it - 1)));
            }
            AllocTraits::destroy(alloc, (it - 1).outerPtr);
        }
        AllocTraits::construct(alloc, pos.outerPtr, value);
        ++sz;
        ++itEnd;
        return pos;
    }

    // gives the same exception safety guarantee as insert (before UPD)
    iterator erase(iterator pos)
    {
        AllocTraits::destroy(alloc, pos.outerPtr);
        for (iterator it = pos + 1; it != itEnd; ++it)
        {
            AllocTraits::construct(alloc, (it - 1).outerPtr, std::move(*it));
            AllocTraits::destroy(alloc, it.outerPtr);
        }
        --sz;
        --itEnd;
        return pos;
    }

    void shrink_to_fit()
    {
        if (!sz) [[unlikely]]
        {
            if (innerBuffer) clear();
            return;
        }
        size_t newBucketCount = (itEnd - 1).innerPtr - itBegin.innerPtr + 2;
        T** newInnerBuffer = InnerAllocTraits::allocate(innerAlloc, 
            newBucketCount);
        for (size_t i = itBegin.innerPtr - innerBuffer; 
            i <= itEnd.innerPtr - innerBuffer; ++i)
        {
            newInnerBuffer[i - (itBegin.innerPtr - innerBuffer)] = 
                innerBuffer[i];
        }
        for (size_t i = 0; i < itBegin.innerPtr - innerBuffer; ++i)
        {
            if (bucketAllocated(i))
            {
                AllocTraits::deallocate(alloc, innerBuffer[i],
                    ELEMENTS_IN_BUCKET);
            }
        }
        for (size_t i = itEnd.innerPtr - innerBuffer + 1; i < bucketCount; ++i)
        {
            if (bucketAllocated(i))
            {
                AllocTraits::deallocate(alloc, innerBuffer[i],
                    ELEMENTS_IN_BUCKET);
            }
        }
        itBegin.innerPtr = newInnerBuffer;
        itEnd.innerPtr = isNullptr(itEnd.outerPtr) ? 
            newInnerBuffer + newBucketCount - 1 :
            newInnerBuffer + newBucketCount - 2;
        InnerAllocTraits::deallocate(innerAlloc, innerBuffer, bucketCount);
        bucketCount = newBucketCount;
        innerBuffer = newInnerBuffer;
    }

    void resize(size_t count, const T& value = T())
    {
        if (count == sz) return; // case tested
        if (!count)
        {
            clear();
            return;
        }
        if (count < sz) // case tested
        {
            iterator newItEnd = itBegin + count;
            for (iterator it = itEnd - 1; it >= newItEnd; --it)
            {
                AllocTraits::destroy(alloc, it.outerPtr);
            }
            for (size_t i = newItEnd.innerPtr - innerBuffer + 1;
                i <= bucketCount - 1; ++i)
            {
                if (!bucketAllocated(i)) continue;
                AllocTraits::deallocate(alloc, innerBuffer[i], 
                    ELEMENTS_IN_BUCKET); 
                innerBuffer[i] = nullptr;
            }
            itEnd = newItEnd;
        } else {
            if (!sz)
            {
                if (!count) return;
                sz = count;
                bucketCount = 2 * BUCKET_RESERVE + 1 + (count +
                    ELEMENTS_IN_BUCKET - 1) / ELEMENTS_IN_BUCKET;
                innerBuffer = InnerAllocTraits::allocate(innerAlloc, 
                    bucketCount);
                initializeElements(value);
                return;
            }
            size_t bucketsRequiered = (count - sz + (itEnd.outerPtr -
                *itEnd.innerPtr) - 1) / ELEMENTS_IN_BUCKET;
            if (itEnd.innerPtr - innerBuffer + bucketsRequiered >
                bucketCount - 2)
            {
                size_t newBucketCount = itEnd.innerPtr - innerBuffer + 
                    bucketsRequiered + 2;
                T** newInnerBuffer = InnerAllocTraits::allocate(innerAlloc,
                    newBucketCount);
                for (size_t i = 0; i < bucketCount - 1; ++i)
                {
                    newInnerBuffer[i] = innerBuffer[i];
                }
                size_t firstAllocatedBucket = static_cast<size_t>(-1);
                size_t i = itEnd.innerPtr - innerBuffer + 1;
                if (isNullptr(itEnd.outerPtr)) --i;
                try
                {
                    for (; i <= newBucketCount - 2; ++i)
                    {
                        if (!isNullptr(newInnerBuffer[i])) continue;
                        if (firstAllocatedBucket == static_cast<size_t>(-1))
                            [[unlikely]]
                        {
                            firstAllocatedBucket = i;
                        }
                        newInnerBuffer[i] = AllocTraits::allocate(alloc,
                            ELEMENTS_IN_BUCKET);
                    }
                } catch (...) { // exc tested
                    for (size_t j = i - 1; j >= firstAllocatedBucket; --j)
                    {
                        AllocTraits::deallocate(alloc, newInnerBuffer[i],
                            ELEMENTS_IN_BUCKET);
                    }
                    InnerAllocTraits::deallocate(innerAlloc, newInnerBuffer,
                        newBucketCount);
                    throw;
                }
                itBegin.innerPtr = newInnerBuffer + (itBegin.innerPtr - 
                    innerBuffer);
                iterator oldItEnd = iterator(newInnerBuffer + (itEnd.innerPtr -
                    innerBuffer), itEnd.outerPtr);
                if (isNullptr(oldItEnd.outerPtr))
                {
                    oldItEnd.outerPtr = *oldItEnd.innerPtr;
                }
                itEnd = itBegin + count;
                iterator it = oldItEnd;
                try
                {
                    for (; it != itEnd; ++it)
                    {
                        AllocTraits::construct(alloc, it.outerPtr, value);
                    }
                } catch (...) { // exc tested
                    for (iterator catchIt = it - 1; catchIt >= oldItEnd; 
                        --catchIt)
                    {
                        AllocTraits::destroy(alloc, catchIt.outerPtr);
                    }
                    for (size_t i = itEnd.innerPtr - newInnerBuffer;
                        i >= firstAllocatedBucket; --i)
                    {
                        AllocTraits::deallocate(alloc, newInnerBuffer[i],
                            ELEMENTS_IN_BUCKET);
                    }
                    InnerAllocTraits::deallocate(innerAlloc, newInnerBuffer,
                        newBucketCount);
                    itBegin.innerPtr = innerBuffer + (itBegin.innerPtr -
                        newInnerBuffer);
                    itEnd = itBegin + sz;
                    throw;
                }
                InnerAllocTraits::deallocate(innerAlloc, innerBuffer, 
                    bucketCount);
                bucketCount = newBucketCount;
                innerBuffer = newInnerBuffer;
                sz = count;
                return;
            } // case tested
            size_t firstAllocatedBucket = static_cast<size_t>(-1);
            size_t i = itEnd.innerPtr - innerBuffer + 1;
            if (isNullptr(itEnd.outerPtr)) --i;
            try
            {
                for (; i <= itEnd.innerPtr - innerBuffer + 
                    bucketsRequiered; ++i)
                {
                    if (bucketAllocated(i)) continue;
                    if (firstAllocatedBucket == static_cast<size_t>(-1))
                        [[unlikely]]
                    {
                        firstAllocatedBucket = i;
                    }
                    innerBuffer[i] = AllocTraits::allocate(alloc, 
                        ELEMENTS_IN_BUCKET);
                }
            } catch (...) { // exc tested
                for (size_t j = i - 1; j >= firstAllocatedBucket; --j)
                {
                    AllocTraits::deallocate(alloc, innerBuffer[j],
                        ELEMENTS_IN_BUCKET);
                    innerBuffer[j] = nullptr;
                }
                throw;
            }
            iterator newItEnd = itBegin + count;
            iterator it = itEnd;
            if (isNullptr(it.outerPtr))
            {
                it.outerPtr = *it.innerPtr;
            }
            try
            {
                for (; it != newItEnd; ++it)
                {
                    AllocTraits::construct(alloc, it.outerPtr, value);
                }
            } catch (...) { // exc tested
                if (isNullptr(itEnd.outerPtr))
                {
                    itEnd.outerPtr = *itEnd.innerPtr;
                }
                for (iterator catchIt = it - 1; catchIt >= itEnd; --catchIt)
                {
                    AllocTraits::destroy(alloc, catchIt.outerPtr);
                }
                for (size_t i = itEnd.innerPtr - innerBuffer + 1 + 
                    bucketsRequiered; i >= firstAllocatedBucket; --i)
                {
                    if (!bucketAllocated(i)) continue;
                    AllocTraits::deallocate(alloc, innerBuffer[i],
                        ELEMENTS_IN_BUCKET);
                    innerBuffer[i] = nullptr;
                }
                throw;
            }
            itEnd = newItEnd;
        }
        sz = count;
    }

    // allocators are considered equal if one allocator can destroy and
    // deallocate what other one constructed and allocated
    void swap(deque& other) noexcept(AllocTraits::is_always_equal::value)
    {
        if constexpr (!AllocTraits::is_always_equal::value)
        {
            std::swap(alloc, other.alloc);
            std::swap(innerAlloc, other.innerAlloc);
        }
        std::swap(sz, other.sz);
        std::swap(bucketCount, other.bucketCount);
        std::swap(innerBuffer, other.innerBuffer);
        std::swap(itBegin, other.itBegin);
        std::swap(itEnd, other.itEnd);
    }

private:
    template <typename... Args>
    reference pushInEmptyContainer(Args&&... args)
    {
        innerBuffer = InnerAllocTraits::allocate(
            innerAlloc, 2 + 2 * BUCKET_RESERVE); // exc tested
        try
        {
            innerBuffer[BUCKET_RESERVE] = 
                AllocTraits::allocate(alloc, ELEMENTS_IN_BUCKET);
        } catch (...) { // exc tested
            InnerAllocTraits::deallocate(
                innerAlloc, innerBuffer, 2 + 2 * BUCKET_RESERVE);
            innerBuffer = nullptr;
            throw;
        }
        try
        {
            AllocTraits::construct(alloc, innerBuffer[BUCKET_RESERVE], 
                std::forward<Args>(args)...);
        } catch (...) { // exc tested
            AllocTraits::deallocate(
                alloc, innerBuffer[BUCKET_RESERVE], ELEMENTS_IN_BUCKET);
            InnerAllocTraits::deallocate(
                innerAlloc, innerBuffer, 2 + 2 * BUCKET_RESERVE);
            innerBuffer = nullptr;
            throw;
        }
        bucketCount = 2 + 2 * BUCKET_RESERVE;
        sz = 1;
        itBegin = iterator(
            innerBuffer + BUCKET_RESERVE, innerBuffer[BUCKET_RESERVE]);
        itEnd = itBegin + 1;
        return innerBuffer[BUCKET_RESERVE][0];
    }

    bool bucketAllocated(size_t i) const noexcept
    {
#ifdef __SANITIZE_ADDRESS__
        return innerBuffer[i] && innerBuffer[i] != (T*)0xbebebebebebebebe;
#else 
        return innerBuffer[i];
#endif
    }
 
    void deallocateBucketsAndInnerBuffer(
        size_t firstBucket, size_t lastBucket) noexcept
    {
        for (size_t i = lastBucket; i >= firstBucket; --i)
        {
            AllocTraits::deallocate(
                alloc, innerBuffer[i], ELEMENTS_IN_BUCKET);
        }
        InnerAllocTraits::deallocate(innerAlloc, innerBuffer, bucketCount);
    }

    void clearHelper(Alloc alloc_,
                     InnerAlloc innerAlloc_,
                     size_t bucketCount_, 
                     T** innerBuffer_, 
                     iterator itBegin_, 
                     iterator itEnd_) noexcept
    {
        if (!innerBuffer) return;
        for (iterator it = itBegin_; it != itEnd_; ++it)
        {
            AllocTraits::destroy(alloc_, it.outerPtr);
        }
        for (size_t i = 0; i < bucketCount_; ++i)
        {
            if (bucketAllocated(i))
            {
                AllocTraits::deallocate(
                    alloc_, innerBuffer_[i], ELEMENTS_IN_BUCKET);
            }
        }
        InnerAllocTraits::deallocate(
            innerAlloc_, innerBuffer_, bucketCount_);
    }

    void clearHelper() noexcept
    {
        return clearHelper(alloc,
                           innerAlloc,
                           bucketCount,
                           innerBuffer, 
                           itBegin,
                           itEnd);
    }

    template <typename ValueT>
    void initializeElements(const ValueT& value)
    {
        if (!sz) return;
        size_t bucket = BUCKET_RESERVE;
        try
        {
            for (; bucket <= BUCKET_RESERVE + (sz - 1) / ELEMENTS_IN_BUCKET;
                ++bucket)
            {
                innerBuffer[bucket] = 
                    AllocTraits::allocate(alloc, ELEMENTS_IN_BUCKET);
            }
        } catch (...) { // exc tested
            deallocateBucketsAndInnerBuffer(BUCKET_RESERVE, bucket - 1);
            throw;
        }
        bucket = BUCKET_RESERVE;
        size_t shift = 0;
        size_t elementsCreated = 0;
        try
        {
            if constexpr (std::is_same_v<T, ValueT>)
            {
                for (; elementsCreated != sz; ++shift, ++elementsCreated)
                {
                    if (shift == ELEMENTS_IN_BUCKET) [[unlikely]]
                    {
                        ++bucket;
                        shift = 0;
                    }
                    AllocTraits::construct(
                        alloc, innerBuffer[bucket] + shift, value);
                }
            } else if constexpr (std::is_same_v<
                    std::initializer_list<T>, ValueT>) {
                auto listIt = value.begin();
                for (; elementsCreated != sz; 
                    ++shift, ++elementsCreated, ++listIt)
                {
                    if (shift == ELEMENTS_IN_BUCKET) [[unlikely]]
                    {
                        ++bucket;
                        shift = 0;
                    }
                    AllocTraits::construct(
                        alloc, innerBuffer[bucket] + shift, *listIt);
                }
            } else {
                static_assert(true);
            }
        } catch (...) { // exc tested
            --shift;
            for (size_t el = elementsCreated; el != 0; --el, --shift)
            {
                if (shift == static_cast<size_t>(-1)) [[unlikely]]
                {
                    --bucket;
                    shift = ELEMENTS_IN_BUCKET - 1;
                }
                AllocTraits::destroy(alloc, innerBuffer[bucket] + shift);
            }
            deallocateBucketsAndInnerBuffer(BUCKET_RESERVE, 
                BUCKET_RESERVE + (sz - 1) / ELEMENTS_IN_BUCKET);
            throw;
        }
        itBegin = iterator(innerBuffer + BUCKET_RESERVE, 
            innerBuffer[BUCKET_RESERVE]);
        itEnd = itBegin + sz;
    }

    void copyBuffer(const deque& other)
    {
        // 1. Allocate buckets as in other
        size_t firstBucket = 0;
        for (; firstBucket < bucketCount; ++firstBucket)
        {
            if (other.bucketAllocated(firstBucket)) break;
        }
        size_t bucket = firstBucket;
        try
        {
            for (; bucket < bucketCount; ++bucket)
            {
                if (!other.bucketAllocated(bucket)) break;
                innerBuffer[bucket] = AllocTraits::allocate(
                    alloc, ELEMENTS_IN_BUCKET);
            }
        } catch (...) { // exc tested
            deallocateBucketsAndInnerBuffer(firstBucket, bucket - 1);
            throw;
        }
        // 2. Copy elements from other
        size_t lastBucket = bucket - 1;
        iterator otherIt = other.itBegin;
        bucket = otherIt.innerPtr - other.innerBuffer;
        size_t shift = otherIt.outerPtr - *otherIt.innerPtr;
        size_t elementsCreated = 0;
        itBegin = iterator(innerBuffer + bucket, innerBuffer[bucket] + shift);
        try
        {
            for (; elementsCreated != sz; 
                ++shift, ++elementsCreated, ++otherIt)
            {
                if (shift == ELEMENTS_IN_BUCKET) [[unlikely]]
                {
                    ++bucket;
                    shift = 0;
                }
                AllocTraits::construct(
                    alloc, innerBuffer[bucket] + shift, *otherIt);
            }
        } catch (...) { // exc tested
            --shift;
            for (size_t el = elementsCreated; el != 0; --el, --shift)
            {
                if (shift == static_cast<size_t>(-1)) [[unlikely]]
                {
                    --bucket;
                    shift = ELEMENTS_IN_BUCKET - 1;
                }
                AllocTraits::destroy(alloc, innerBuffer[bucket] + shift);
            }
            deallocateBucketsAndInnerBuffer(firstBucket, lastBucket);
            throw;
        }
        // 3. Initialize itBegin(earlier) and itEnd
        itEnd = itBegin + sz;
    }

private:
    Alloc alloc;
    InnerAlloc innerAlloc;
    size_t sz;
    size_t bucketCount;
    T** innerBuffer;
    iterator itBegin;
    iterator itEnd;
};

template <typename It>
It operator+(int64_t num, const It& it)
{
    It res = it;
    res += num;
    return res;
}

/* In the following operator+ implementation type deduction fails: 
 */
//   template <typename T, typename Alloc>
//   typename deque<T, Alloc>::iterator operator+(long long int num, 
//       typename deque<T, Alloc>::iterator it)
//   {
//       typename deque<T, Alloc>::iterator res = it;
//       res += num;
//       return res;
//   }

#endif
