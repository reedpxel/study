#include <iostream>
#include <exception>
#include <cmath>

template <typename T>
class Heap
{
protected:
    T* buffer;
    size_t default_size;
    size_t buffer_size;
    size_t sz;

    virtual void sift_up(size_t i) = 0;
    virtual void sift_down(size_t i) = 0;

public:

    Heap();
    Heap(size_t _sz);
    ~Heap();
    void insert(T _key);
    void clear();
    size_t size() const;
    void print() const;
    T extract_top();
    T get_top() const;
};

template <typename T>
class MinHeap : public Heap<T>
{
    void sift_up(size_t i) override;
    void sift_down(size_t i) override;
public:
    MinHeap();
    MinHeap(size_t sz);
};

template <typename T>

class MaxHeap : public Heap<T>
{
    void sift_up(size_t i) override;
    void sift_down(size_t i) override;
public:
    MaxHeap();
    MaxHeap(size_t sz);
};

template <typename T>
Heap<T>::Heap(): buffer(new T[16]), default_size(16), buffer_size(16), sz(0) {}

template <typename T>
Heap<T>::Heap(size_t _sz): buffer(new T[_sz]), default_size(_sz), buffer_size(_sz), sz(0) {}

template <typename T>
Heap<T>::~Heap() { delete[] buffer; }

template <typename T>
void Heap<T>::insert(T _key)
{
    if (sz == buffer_size)
    {
        T* new_buffer = new T[2 * buffer_size];
        for (size_t i = 0; i < buffer_size; ++i)
        {
            new_buffer[i] = buffer[i];
        }
        delete[] buffer;
        buffer = new_buffer;
        buffer_size *= 2;
    }
    buffer[sz] = _key;
    ++sz;
    sift_up(sz - 1);
}

template <typename T>
void Heap<T>::clear()
{
    delete[] buffer;
    T* new_buffer = new T[default_size];
    buffer = new_buffer;
    sz = 0;
}

template <typename T>
size_t Heap<T>::size() const { return sz; }

template <typename T>
void Heap<T>::print() const
{
    for (size_t i = 0; i < sz; ++i)
    {
        std::cout << buffer[i] << ' ';
    }
    std::cout << '\n';
}

template <typename T>
T Heap<T>::get_top() const
{
    if (!sz) throw std::length_error("size == 0");
    return buffer[0];
}

template <typename T>
T Heap<T>::extract_top()
{
    if (!sz) throw std::length_error("size == 0");
    T top = buffer[0];
    buffer[0] = buffer[sz - 1];
    --sz;
    if (sz) sift_down(0);
    return top;
}

template <typename T>
MinHeap<T>::MinHeap(): Heap<T>::Heap() {}

template <typename T>
MaxHeap<T>::MaxHeap(): Heap<T>::Heap() {}

template <typename T>
MinHeap<T>::MinHeap(size_t sz): Heap<T>::Heap(sz) {}

template <typename T>
MaxHeap<T>::MaxHeap(size_t sz): Heap<T>::Heap(sz) {}

template <typename T>
void MinHeap<T>::sift_up(size_t i)
{
    if (i >= this->sz) throw std::out_of_range("i >= this->sz");
    while (i)
    {
        size_t j = (i - 1) / 2;
        if (this->buffer[i] < this->buffer[j])
        {
            T tmp = this->buffer[i];
            this->buffer[i] = this->buffer[j];
            this->buffer[j] = tmp;
        }
        i = j;
    }
}

template <typename T>
void MinHeap<T>::sift_down(size_t i)
{
    if (i >= this->sz) throw std::out_of_range("i >= this->sz");
    size_t min = i;
    if ((this->buffer[i] > this->buffer[2 * i + 1] && 2 * i + 1 < this->sz) || 
        (this->buffer[i] > this->buffer[2 * i + 2] && 2 * i + 2 < this->sz))
    {
        min = this->buffer[2 * i + 1] < this->buffer[2 * i + 2] ? 2 * i + 1 : 2 * i + 2;
    }
    if (min != i)
    {
        T tmp = this->buffer[i];
        this->buffer[i] = this->buffer[min];
        this->buffer[min] = tmp;
        sift_down(min);
    }
}

template <typename T>
void MaxHeap<T>::sift_up(size_t i)
{
    if (i >= this->sz) throw std::out_of_range("i >= this->sz");
    while (i)
    {
        size_t j = (i - 1) / 2;
        if (this->buffer[i] > this->buffer[j])
        {
            T tmp = this->buffer[i];
            this->buffer[i] = this->buffer[j];
            this->buffer[j] = tmp;
        }
        i = j;
    }
}

template <typename T>
void MaxHeap<T>::sift_down(size_t i)
{
   if (i >= this->sz) throw std::out_of_range("i >= this->sz");
    size_t min = i;
    if ((this->buffer[i] < this->buffer[2 * i + 1] && 2 * i + 1 < this->sz) || 
        (this->buffer[i] < this->buffer[2 * i + 2] && 2 * i + 2 < this->sz))
    {
        min = this->buffer[2 * i + 1] > this->buffer[2 * i + 2] ? 2 * i + 1 : 2 * i + 2;
    }
    if (min != i)
    {
        T tmp = this->buffer[i];
        this->buffer[i] = this->buffer[min];
        this->buffer[min] = tmp;
        sift_down(min);
    }
}

int main()
{
    MaxHeap<int> h;
    h.insert(1);
    h.clear();
    std::cout << h.get_top();
}
