#include <iostream>
#include <vector>

// решение static rsq online/offline. Массив префиксных сумм строится за О(n), ответ получается за О(1)

template <typename T>
T rsq(std::vector<T> v, int l, int r)
{
    std::vector<T> pref_sum = {0};
    T tmp = v[0];
    pref_sum.push_back(tmp);
    for (int i = 1; i < v.size(); ++i)
    {
        tmp += v[i];
        pref_sum.push_back(tmp);
    }
    return pref_sum[r + 1] - pref_sum[l];
}

int main()
{
    std::vector<int> v = {2, -3, 7, 1, 4, 5, -8, 1, 3};
    std::cout << rsq(v, 6, 7);
}