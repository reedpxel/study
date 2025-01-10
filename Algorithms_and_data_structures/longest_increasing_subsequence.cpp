#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <deque>
#include <climits>

// Решение задачи поиска наибольшей возрастающей подпоследовательности
// 1) нахождение длины за O(n ^ 2)
// 2) нахождение длины за O(n * log(n))
// 3) нахождение последовательности за O(n * log(n))
// 4) жадный алгоритм

int parent(int x) { return (x - 1) / 2; }

int left_child(int x) { return 2 * x + 1; }

int right_child(int x) { return 2 * x + 2; }

int max(int a, int b) { return a >= b ? a : b; }

int get_LIS_size(std::vector<int>& v) // за О(n ^ 2)
{
    if (v.empty()) return 0;
    std::vector<int> dp;
    dp.reserve(v.size());
    dp.push_back(1);
    for (int i = 1; i < v.size(); ++i)
    {
        int max = 1;
        for (int j = 0; j < i; ++j)
        {
            if (v[i] > v[j] && dp[j] + 1 > max) max = dp[j] + 1;
        }
        dp.push_back(max);
    }
    int res = 1;
    // for (auto x : dp) std::cout << x << ' ';
    // std::cout << '\n';
    for (auto x : dp)
    {
        if (res < x) res = x;
    }
    return res;
}

void update(std::vector<int>& heap, int index, int value)
{
    index += heap.size() / 2;
    heap[index] = value;
    int parent_index = parent(index);
    while (index)
    {
        if (left_child(parent_index) == index) heap[parent_index] = max(heap[index], heap[right_child(parent_index)]);
        else heap[parent_index] = max(heap[left_child(parent_index)], heap[index]);
        index = parent_index;
        parent_index = parent(parent_index);
    }
}

int get_max_rec(std::vector<int>& heap, int l, int r, int i)
{
    int left = left_child(i);
    int right = right_child(i);
    size_t sz_half = heap.size() / 2;
    if (i >= sz_half)
    {
        left = right = i - sz_half;
    } else {
        while (left < sz_half) left = left_child(left);
        while (right < sz_half) right = right_child(right);
        left -= sz_half;
        right -= sz_half;
    }
    if (right < l || left > r) return 0;
    else if (l <= left && right <= r) return heap[i];
    else return max(get_max_rec(heap, l, r, left_child(i)), get_max_rec(heap, l, r, right_child(i)));
}

int get_max(std::vector<int>& heap, int l, int r) { return get_max_rec(heap, l, r, 0); }

int get_LIS_size_optimized(std::vector<int>& v) // алгоритм за O (n logn), использует дерево отрезков
{
    std::vector<std::pair<int, int>> pairs;
    pairs.reserve(v.size());
    for (int i = 0; i < v.size(); ++i) pairs.push_back(std::pair<int, int>(v[i], i));
    std::sort(pairs.begin(), pairs.end(), [](auto& x, auto& y)
    {
        if (x.first != y.first) return x.first < y.first;
        return x.second > y.second;
    });
    // for (auto x : pairs) std::cout << x.first << '_' << x.second << ' ';
    // std::cout << '\n';
    std::vector<int> heap;
    heap.resize(pow(2, 1 + ceil(log2(v.size()))) - 1, INT_MIN);
    for (int i = 0; i < pairs.size(); ++i)
    {
        update(heap, pairs[i].second, max(1, 1 + get_max(heap, 0, pairs[i].second)));
        // for (int i = heap.size() / 2; i < heap.size(); ++i) std::cout << heap[i] << ' ';
        // std::cout << '\n';
    }
    return get_max(heap, 0, v.size());
}

std::vector<int> get_LIS(std::vector<int>& v) // возвращает саму максимальную возрастающую последовательность. O (n * logn)
{
    std::vector<std::pair<int, int>> pairs;
    pairs.reserve(v.size());
    for (int i = 0; i < v.size(); ++i) pairs.push_back(std::pair<int, int>(v[i], i));
    std::sort(pairs.begin(), pairs.end(), [](auto& x, auto& y)
    {
        if (x.first != y.first) return x.first < y.first;
        return x.second > y.second;
    });
    std::vector<int> heap;
    heap.resize(pow(2, 1 + ceil(log2(v.size()))) - 1, INT_MIN);
    for (int i = 0; i < pairs.size(); ++i)
    {
        update(heap, pairs[i].second, max(1, 1 + get_max(heap, 0, pairs[i].second)));
    }
    int current_sz = get_max(heap, 0, v.size());
    std::deque<int> res;
    for (int i = heap.size() - 1; i > 0; --i)
    {
        if (heap[i] == current_sz)
        {
            res.push_back(v[i - heap.size() / 2]);
            --current_sz;
        }
        if (!current_sz) break;
    }
    std::reverse(res.begin(), res.end());
    return std::vector<int>(res.begin(), res.end());
}

// жадный алгоритм с бинарным поиском. С учетом существования этого алгоритма,
// алгоритм с использованием ДП не имеет смысла, потому что ассимптотическая
// сложность та же, а никакое дерево отрезков инициализировать не надо
// UPD: этот алгоритм тоже можно считать использующим динамическое
// программирование. subsequence[i] это наименьший элемент, на который 
// кончается возрастающая подпоследовательность длиной i
int get_LIS_size_greedy(const std::vector<int>& v)
{
    std::vector<int> subsequence;
    for (int i : v)
    {
        if (!subsequence.size() || subsequence.back() < i)
        {
            subsequence.push_back(i);
        } else {
            auto first_bigger_or_equal = std::lower_bound(subsequence.begin(),
                    subsequence.end(), i);
            *first_bigger_or_equal = i;
        }
    }
    return subsequence.size(); // the subsequence itself is returned if required
}

int main()
{
    std::vector<int> vv = {-1, 0, -1, 2, 3, 1};
    std::cout << get_LIS_size_greedy(vv);
}
