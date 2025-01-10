#include <iostream>
#include <vector>
#include <cmath>
#include <functional>

// построение дерева отрезков за O(N), получение ответа за O(logN)

// template <typename T>
// T dynamic_rsq_rec(std::vector<T> v, size_t l, size_t r, size_t sz, size_t index);

// template <typename T>
// T dynamic_rsq(std::vector<T> v, size_t l, size_t r)
// {
//     double log_sz = log2(v.size());
//     if (log_sz - floor(log_sz))
//     {
//         size_t new_sz = pow(2, ceil(log_sz));
//         while (v.size() < new_sz) v.push_back(0);
//     }
//     size_t sz = v.size();
//     size_t _l = 0;
//     size_t _r = sz - 1;
//     while (_l != _r)
//     {
//         for (size_t i = _l; i < _r; i += 2)
//         {
//             v.push_back(v[i] + v[i + 1]);
//         }
//         _l = _r + 1;
//         _r = v.size() - 1;
//     }
//     return dynamic_rsq_rec(v, l, r, sz, v.size() - 1);
// }

// template <typename T>
// T dynamic_rsq_rec(std::vector<T> v, size_t l, size_t r, size_t sz, size_t index)
// {
//     size_t left, right;
//     if (index < sz)
//     {
//         left = right = index;
//     } else {
//         size_t sz_tmp = sz;
//         size_t sz_prev = sz;
//         size_t _sz = sz / 2;
//         size_t dist = 2;
//         while (_sz > 1)
//         {
//             sz_tmp += _sz;
//             if (index < sz_tmp)
//             {
//                 left = (index - sz_prev) * dist;
//                 right = left + dist - 1;
//                 break;
//             }
//             dist *= 2;
//             _sz /= 2;
//             sz_prev = sz_tmp;
//         }
//         if (index == sz_tmp)
//         {
//             left = 0;
//             right = dist - 1;
//         }
//     }
//     if (right < l || left > r) return 0;
//     else if (l <= left && right <= r) return v[index];
//     else return dynamic_rsq_rec(v, l, r, sz, v.size() - 1 - (2 * (v.size() - 1- index) + 2)) + 
//         dynamic_rsq_rec(v, l, r, sz, v.size() - 1 - (2 * (v.size() - 1 - index) + 1)) ;
// }

template <typename T>
T sum(T x, T y) { return x + y; }

template <typename T>
std::vector<T> build_heap(std::vector<T> v, std::function<T(T, T)> operation = sum<T>, T neutral_element = 0)
{
    double log_size = log2(v.size());
    if (floor(log_size) != log_size)
    {
        for (size_t i = v.size(); i < pow(2, ceil(log_size)); ++i)
        {
            v.push_back(neutral_element);
        }
    }
    for (size_t i = v.size() - 1; i > 0; --i)
    {
        v.insert(v.begin(), operation(v[i - 1], v[i]));
    }
    return v;
}

template <typename T>
T get_ans_rec(std::vector<T> heap, size_t l, size_t r, size_t index, std::function<T(T, T)> operation = sum<T>, T neutral_element = 0)
{
    size_t left;
    size_t right;
    if (index >= heap.size() / 2)
    {
        left = right = index - heap.size() / 2;
    } else {
        left = 2 * index + 1;
        right = 2 * index + 2;
        while (left < heap.size() / 2) left = 2 * left + 1;
        while (right < heap.size() / 2) right = 2 * right + 2;
        left -= heap.size() / 2;
        right -= heap.size() / 2;
    }
    if (right < l || left > r) return neutral_element;
    else if (l <= left && right <= r) return heap[index];
    else return operation(get_ans_rec(heap, l, r, 2 * index + 1, operation, neutral_element), 
        get_ans_rec(heap, l, r, 2 * index + 2, operation, neutral_element));
}

template <typename T>
T get_ans(std::vector<T> heap, size_t l, size_t r, std::function<T(T, T)> operation = sum<T>, T neutral_element = 0)
{
    return get_ans_rec(heap, l, r, 0, operation, neutral_element);
}

template <typename T>
void add(std::vector<T>& v, size_t index, T num)
{
    v[index] += num;
}

template <typename T>
void update(std::vector<int>& heap, size_t index, T num, std::function<void(std::vector<T>&, size_t, T)> operation = add<T>)
{
    index += heap.size() / 2;
    while (index < heap.size())
    {
        operation(heap, index, num);
        index = (index - 1) / 2;
    }
}

int main()
{
    std::vector<int> v = {7, 1, -4, 1, -3, 4, 1, 2};
    std::function<int(int, int)> min = [](int x, int y){ return x < y ? x : y; };
    std::vector<int> heap = build_heap(v, min, INT_MAX);
    for (auto x : heap) std::cout << x << ' ';
    std::cout << '\n';
    std::cout << get_ans(heap, 0, 8, min, INT_MAX);
    std::function<void(std::vector<int>&, size_t, int)> equal = [](std::vector<int>& v, size_t index, int num){ v[index] = num; };
    update(heap, 7, 1000, equal);
    for (auto x : heap) std::cout << x << ' ';
    std::cout << '\n';
    std::cout << '\n' << get_ans(heap, 0, 8, min, INT_MAX);
}