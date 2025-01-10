#include <iostream>
#include <memory>
#include <exception>

struct C
{
    size_t x;

    C(size_t x) : x(x)
    {
        if (x == 2) throw std::invalid_argument("An exception in constructor");
    }
    C(const C& _c)
    {
        if (_c.x == 1)
        {
            throw std::invalid_argument("An exception in copy constructor\n");
        }
        x = _c.x;
    }
};

template <typename T>
class Vector
{
    T* arr;
    size_t sz;
    size_t cap;

public:
    Vector();
    Vector(size_t amount, const T& element);
    Vector(const std::initializer_list<T>& list);
    ~Vector();

    void reserve(size_t n);
    void resize(size_t n, const T& element = T());
    void push_back(const T& element);
    void pop_back();
    size_t size() const;
    size_t capacity() const;
    T& operator[](size_t i);
    T& at(size_t i);
    const T& operator[](size_t i) const;
    const T& at(size_t i) const;
/*
    1) Почему после неконстантных оператора= и at нет const? 
    2) Почему нужны константный и неконстантный оператор[]?

    1) Для перегрузки оператора= и at для константных и 
неконстантных векторов (у константного вызывается константный 
метод\оператор, т.е. слово const после круглых скобок и его 
отсутствие - предмет перегрузки). Для неконстантного вектора 
они работали бы правильно, если написать const, т.к. при вызове 
"v[2] = 123;" у неконстантого вектора редактирование происходит 
по неконстантной ссылке, а не по самому оператору.
    2) Для того, чтобы перегрузить их для константного и 
неконстантного вектора, так как в С++ нет перегрузки по 
возвращаемому объекту (а возвращать они должны разное - 
неконстантную/константную ссылку).  Можно написать один 
оператор= вида
    T& operator[](size_t i) const { return *(arr + i); },
но этот оператор будет правильно работать только для 
неконстантного вектора, у константного таким оператором[] 
получится поменять значение по квадратным скобкам
*/
};

template <>
class Vector<bool>
{
    struct BitReference
    {
        uint8_t* bucketPtr;
        uint8_t pos;

        BitReference(uint8_t* ptr, uint8_t pos);
        BitReference& operator=(bool b);
        operator bool() const;
    };

    uint8_t* arr;
    size_t sz;
    size_t cap;

public:
    Vector();
    Vector(size_t amount, bool element);
    Vector(const std::initializer_list<bool>& list);
    ~Vector();

    void reserve(size_t n);
    void resize(size_t n, bool element = false);
    void push_back(bool element);
    void pop_back();
    size_t size() const;
    size_t capacity() const;
    BitReference operator[](size_t i);
    BitReference at(size_t i);
    const BitReference operator[](size_t i) const;
    const BitReference at(size_t i) const;
};

template <typename T>
Vector<T>::Vector() : 
    arr(nullptr),
    sz(0),
    cap(0) {}

template <typename T>
Vector<T>::Vector(size_t amount, const T& element) :
    arr(reinterpret_cast<T*>(new uint8_t[amount * sizeof(T)])),
    sz(amount),
    cap(amount)
{
    size_t i = 0;
    try
    {
        for (; i < sz; ++i)
        {
            new (arr + i) T(element);
        }
    } catch(...) {
        for (size_t j = 0; j < i; ++j)
        {
            (arr + i)->~T();
        }
        delete[] reinterpret_cast<uint8_t*>(arr);
        throw;
    }
}

template <typename T>
Vector<T>::Vector(const std::initializer_list<T>& list) :
    arr(reinterpret_cast<T*>(new uint8_t[list.size() * sizeof(T)])),
    sz(list.size()),
    cap(list.size())
{
    try
    {
        std::uninitialized_copy(list.begin(), list.end(), arr);
    } catch(...) {
        delete[] reinterpret_cast<uint8_t*>(arr);
        throw;
    }
}

template <typename T>
Vector<T>::~Vector()
{
    for (size_t i = 0; i < sz; ++i)
    {
        (arr + i)->~T();
    }
    delete reinterpret_cast<uint8_t*>(arr);
}

template <typename T>
void Vector<T>::reserve(size_t n)
{
    if (!arr)
    {
        arr = reinterpret_cast<T*>(new uint8_t[n * sizeof(T)]);     
        cap = n;
        return;
    }
    if (cap >= n) return;
    // T* newArr = new T[n];
/*
как выше нельзя, т.к. у T может не быть конструктора по умолчанию,
он вызывается в new n раз, а потом в ячейки массива вызывается копиру-
ющий оператор=. А даже если конструктор по умолчанию есть, то его может 
быть нельзя вызывать лишний раз (если T это потоки\сетевые соединения\
мьютексы etc.). Нужно выделить память под объекты, а их конструкторы
не вызывать
*/
    T* newArr = reinterpret_cast<T*>(new uint8_t[n * sizeof(T)]);

/*
    size_t i = 0;
    try
    {
        for (; i < sz; ++i)
        {
            // newArr[i] = arr[i];
//как выше нельзя после того, как выделили память при помощи 
//reinterpret_cast-а, т.к., получается, пытаемся вызвать копирующий 
//оператор= от объектов, которые на самом деле не созданы - это UB
            // new (new newArr + i) T(arr[i]);
        }
    } catch(...) {
        for (size_t j = 0; j < i; ++j)
        {
            (newArr + i)->~T(); // или newArr[i]->~T();
        }
        delete[] reinterpret_cast<uint8_t*>(newArr);
        throw;
    }
    */

/*
в <memory> есть функция, где написанное выше уже реализовано - 
std::uninitialized_copy
*/
    try
    {
        std::uninitialized_copy(arr, arr + sz, newArr);
    } catch(...) {
        delete[] reinterpret_cast<uint8_t*>(newArr);
        throw;
    }
    // delete[] arr;
/*
как выше нельзя, т.к. выделяли new от uint8_t, у фундаментальных типов нет
деструкторов => просто освободится память без вызова деструкторов
*/
    delete[] reinterpret_cast<uint8_t*>(arr);
    arr = newArr;
    cap = n;
}

template <typename T>
void Vector<T>::resize(size_t n, const T& element)
{
    if (!arr) arr = reinterpret_cast<T*>(new uint8_t[n * sizeof(T)]);
    if (sz > n)
    {
        sz = n;
        return;
    }
    if (sz < n)
    {
        if (cap < n) reserve(n);
        size_t i = sz;
        try
        {
            for (; i < n; ++i)
            {
                new (arr + i) T(element);
// нельзя arr[sz] = element. Так можно было бы обратиться к 
// элементу, который уже создан, но по arr + sz не вызван конструктор, надо 
// использовать placement new
            }
        } catch(...) {
            for (size_t j = sz; j < i; ++j)
            {
                (arr + j)->~T();
            }
            throw;
        }
        sz = n;
    }
}

template <typename T>
void Vector<T>::push_back(const T& element)
{
    if (!arr) reserve(1);
    if (sz == cap) reserve(2 * cap);
    try
    {
        new (arr + sz) T(element);
    } catch(...) {
        throw;
    }
    ++sz;
}

template <typename T>
void Vector<T>::pop_back()
{
    if (sz == 0) return;
    --sz;
    (arr + sz)->~T();
}

template <typename T>
size_t Vector<T>::size() const { return sz; }

template <typename T>
size_t Vector<T>::capacity() const { return cap; }

template <typename T>
T& Vector<T>::operator[](size_t i) { return *(arr + i); }

template <typename T>
T& Vector<T>::at(size_t i)
{
    if (i >= sz) throw std::out_of_range("Wrong index\n");
    return *(arr + i);
}

template <typename T>
const T& Vector<T>::operator[](size_t i) const { return *(arr + i); }

template <typename T>
const T& Vector<T>::at(size_t i) const
{
    if (i >= sz) throw std::out_of_range("Wrong index\n");
    return *(arr + i);
}

Vector<bool>::BitReference::BitReference(uint8_t* ptr, uint8_t pos) :
    bucketPtr(ptr),
    pos(pos) {}

typename Vector<bool>::BitReference& 
    Vector<bool>::BitReference::operator=(bool b)
{
    if (b) *bucketPtr |= 1 << (7 - pos);
    else *bucketPtr &= ~(1 << (7 - pos));
    return *this;
}

Vector<bool>::BitReference::operator bool() const
{
    return *bucketPtr & (1 << (7 - pos));
}

Vector<bool>::Vector() :
    arr(nullptr),
    sz(0),
    cap(0) {}

Vector<bool>::Vector(size_t amount, bool element) :
    arr(new uint8_t[(amount + 8) / 8]),
    sz(amount),
    cap((amount / 8 + 1) * 8)
{
    for (size_t i = 0; i < (amount + 8) / 8; ++i)
    {
        if (amount == true) arr[i] = 255;
        else arr[i] = 0;
    }
}

Vector<bool>::Vector(const std::initializer_list<bool>& list) :
    arr(new uint8_t[(list.size() + 8) / 8]),
    sz(list.size()),
    cap((list.size() / 8 + 1) * 8)
{
// можно просто через BitReference
    size_t i = 0;
    uint8_t element = 0;
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        if (*it) element |= 1 << (8 - i % 8 - 1);
        if (i % 8 == 7)
        {
            arr[i / 8] = element;
            element = 0;
        }
        ++i;
    }
    if (i % 8) arr[i / 8] = element;
}

Vector<bool>::~Vector() { delete[] arr; }

void Vector<bool>::reserve(size_t n)
{
    if (!arr)
    {
        arr = new uint8_t[(n + 8) / 8];
        cap = (n + 8) / 8 * 8;
        return;
    }
    if (cap >= n) return;
    uint8_t* newArr = new uint8_t[(n + 7) / 8];
    for (size_t i = 0; i < (sz + 8) / 8; ++i)
    {
        newArr[i] = arr[i];
    }
    delete[] arr;
    arr = newArr;
    cap = (n + 7) / 8 * 8;
}

void Vector<bool>::resize(size_t n, bool element)
{
    if (n > sz)
    {
        reserve(n);
        for (size_t i = sz; i < n; ++i)
        {
            BitReference bitR(arr + i / 8, i % 8);
            bitR = element;
        }
        sz = n;
        return;
    }
    if (n < sz) sz = n;
}

void Vector<bool>::push_back(bool element)
{
    if (!arr) reserve(1);
    if (sz == cap) reserve(2 * cap);
    BitReference bitR(arr + sz / 8, sz % 8);
    bitR = element;
    ++sz;
}

void Vector<bool>::pop_back() { if (sz) --sz; }

size_t Vector<bool>::size() const { return sz; }

size_t Vector<bool>::capacity() const { return cap; }

typename Vector<bool>::BitReference Vector<bool>::operator[](size_t i)
{
    return BitReference(arr + i / 8, i % 8);
}

typename Vector<bool>::BitReference Vector<bool>::at(size_t i)
{
    if (i >= sz) throw std::out_of_range("Wrong argument\n");
    return BitReference(arr + i / 8, i % 8);
}

const typename Vector<bool>::BitReference
    Vector<bool>::operator[](size_t i) const
{
    return BitReference(arr + i / 8, i % 8);
}

const typename Vector<bool>::BitReference Vector<bool>::at(size_t i) const
{
    if (i >= sz) throw std::out_of_range("Wrong argument\n");
    return BitReference(arr + i / 8, i % 8);
}

int main()
{
    const Vector<bool> v = {true, true, false, false, true};
    for (int i = 0; i < v.size(); ++i) std::cout << v.at(i);
}
