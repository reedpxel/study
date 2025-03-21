#include <iostream>
#include <algorithm>

// Signed integers used in all algorithms, because in some cases index may
// become -1 and condition left <= right will not interrupt the while cycle.

// Returns the first index i : arr[i] == value that was met during the while 
// cycle. Returns -1 if the element is absent.
int binary_search(int* arr, int sz, int value)
{
    int left = 0, right = sz - 1;
    while (left <= right)
    {
        int middle = left + (right - left) / 2;
        if (arr[middle] < value)
        {
            left = middle + 1;
        } else if (arr[middle] == value) {
            return middle;
        } else {
            right = middle - 1;
        }
    }
    return -1;
}

// Returns an index to the leftmost element equal to value. If there are no
// equal elements, returns index to the biggest element less than value
int lower_bound(int* arr, int sz, int value)
{
    int left = 0, right = sz - 1;
    while (left <= right)
    {
        int middle = left + (right - left) / 2;
        if (arr[middle] > value)
        {
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }
    return left - 1;
}

// Returns index to the first element bigger than value
int upper_bound(int* arr, int sz, int value)
{
    int left = 0, right = sz;
    int res = sz;
    while (left <= right)
    {
        int middle = left + (right - left) / 2;
        if (arr[middle] <= value)
        {
            left = middle + 1;
        } else {
            res = std::min(res, middle);
            right = middle - 1;
        }
    }
    return res;
}

// Returns the leftmost element equal to value. If no equal elements found,
// returns -1.
int firstOne(int* arr, int sz, int value)
{
    int left = 0, right = sz - 1;
    int res = sz;
    while (left <= right)
    {
        int middle = left + (right - left) / 2;
        if (arr[middle] < value)
        {
            left = middle + 1;
        } else if (arr[middle] == value) {
            res = std::min(res, middle);
            right = middle - 1;
        } else {
            right = middle - 1;
        }
    }
    return res == sz ? -1 : res;
}

// Returns rightmost element equal to value. If not equal elements found,
// returns -1
int lastOne(int* arr, int sz, int value)
{
    int left = 0, right = sz - 1;
    int res = -1;
    while (left <= right)
    {
        int middle = left + (right - left) / 2;
        if (arr[middle] < value)
        {
            left = middle + 1;
        } else if (arr[middle] == value) {
            res = std::max(res, middle);
            left = middle + 1;
        } else {
            right = middle - 1;
        }
    }
    return res;
}

int main()
{
    constexpr int array_size = 10;
    int array[array_size] = {-2, 3, -1, -1, 3, 0, -1, -4, -1, -7}; 
    std::sort(array, array + array_size);
    for (int i = 0; i < array_size; ++i)
    {
        std::cout << array[i] << ' ';
    }
    std::cout << '\n';
    for (int value = -10; value <= 10; ++value)
    {
        std::cout << value << ' ';
        std::cout << binary_search(array, array_size, value) << ' ';
        std::cout << lower_bound(array, array_size, value) << ' ';
        std::cout << upper_bound(array, array_size, value) << ' ';
        std::cout << firstOne(array, array_size, value) << ' ';
        std::cout << lastOne(array, array_size, value) << ' ';
        std::cout << std::endl;
    }
}
