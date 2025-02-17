#include "shared_ptr.hpp" 

template <typename T>
class weak_ptr
{
    weak_ptr() noexcept;
    weak_ptr(const shared_ptr& shptr) noexcept;
    weak_ptr(const weak_ptr& other) noexcept;
    weak_ptr(weak_ptr&& other) noexcept;
    ~weak_ptr();
    weak_ptr& operator=(const weak_ptr& other) noexcept;
    weak_ptr& operator=(const shared_ptr& other) noexcept;
    weak_ptr& operator=(weak_ptr&& other) noexcept;
    void reset() noexcept;
    void swap(weak_ptr& other) noexcept;
    size_t use_count() const noexcept; // amount of shared_ptrs that point to
                                       // the owned object
    bool expired() const noexcept; // check whether object is deleted
    shared_ptr lock() const noexcept; // creates a shared_ptr that manages the
                                      // object. Returns expired ? shared_ptr()
                                      // : shared_ptr(*this)
    template <typename U>
    bool owner_before(const weak_ptr& other) const noexcept;
    
    template <typename U>
    bool owner_before(const shared_ptr& other) const noexcept;
};

