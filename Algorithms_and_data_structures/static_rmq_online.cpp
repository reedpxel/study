#include <iostream>
#include <vector>
#include <cmath>

// Решение задачи static RMQ online/offline. Разреженная таблица строится за О(n * log(n))
// (т. к. каждый эл-т считается за О(1), эл-тов n * log2(n)), ответ получается за O(1)

template <typename T>
T min(T x, T y) { return x < y ? x : y; }

template <typename T>
T rmq(const std::vector<T>& v, int l, int r)
{
    std::vector<std::vector<T>> st;
    st.push_back(v);
    int j = 1;
    while (pow(2, j) - 1 < v.size())
    {
        std::vector<T> tmp;
        for (int i = 0; i < v.size() - pow(2, j) + 1; ++i)
        {
            tmp.push_back(min(st[j - 1][i], st[j - 1][i + pow(2, j - 1)]));
        }
        st.emplace_back(tmp);
        ++j;
    }
    int range = r - l;
    if (!range) return v[l];
    return min(st[log2(range)][l], st[log2(range)][r - pow(2, floor(log2(range))) + 1]);
}

// получение индекса минимального эл-та, а не самого эл-та
template <typename T>
int rmq_index(const std::vector<T>& v, int l, int r)
{
    std::vector<std::vector<int>> sparsed_table(1, std::vector<int>(v.size()));
    for (int i = 0; i < v.size(); ++i) sparsed_table[0][i] = i;
    int log_sz = log2(v.size());
    for (int i = 1; i < log_sz + 1; ++i)
    {
        int tmp_sz = v.size() - (1 << i) + 1; // 1 << j <=> pow(2, j)
        std::vector<int> tmp(tmp_sz);
        for (int j = 0; j < tmp_sz; ++j)
        {
            tmp[j] = v[sparsed_table[i - 1][j]] < 
                v[sparsed_table[i - 1][j + (1 << (i - 1))]] ? 
                sparsed_table[i - 1][j] : 
                sparsed_table[i - 1][j + (1 << (i - 1))];
        }
        sparsed_table.emplace_back(tmp);
    }
    if (l == r) return l;
    int k = log2(r - l);
    if (v[sparsed_table[k][l]] < v[sparsed_table[k][r - (1 << k) + 1]])
    {
        return sparsed_table[k][l];
    }
    return sparsed_table[k][r - (1 << k) + 1];
}

int main()
{
    std::vector<int> a = {2, -1, 7, 3, -2, 5, 4, -3, 2};
    // std::cout << rmq(a, a.size() - 4, a.size() - 2);
    std::cout << rmq_index(a, 0, 8);
}