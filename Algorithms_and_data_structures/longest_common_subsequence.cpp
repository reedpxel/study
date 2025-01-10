#include <iostream>
#include <vector>
#include <algorithm>

// Алгоритм поиска наибольшей общей подпоследовательности. Работает за O(v1.size() * v2.size())

int max(int a, int b) { return a > b ? a : b; }

int max(int a, int b, int c) { return max(max(a, b), c); }

int get_LCS_size(std::vector<int>& v1, std::vector<int>& v2)
{
    if (v1.empty() || v2.empty()) return 0;
    int sz1 = v1.size(); // размер одного вектора
    int sz2 = v2.size(); // количество векторов в dp
    // std::cout << sz1 << ' ' << sz2 << '\n';
    std::vector<std::vector<int>> dp;
    std::vector<int> tmp;
    tmp.resize(sz1);
    dp.resize(sz2, tmp);
    for (int i = 0; i < sz2; ++i)
    {
        if (v1[0] == v2[i]) dp[i][0] = 1;
    }
    for (int i = 1; i < sz1; ++i)
    {
        if (v1[i] == v2[0]) dp[0][i] = 1;
    }
    for (int i = 1; i < sz2; ++i)
    {
        for (int j = 1; j < sz1; ++j)
        {
            int diag = dp[i - 1][j - 1];
            if (v1[j] == v2[i]) ++diag;
            int left = dp[i - 1][j];
            int top = dp[i][j - 1];
            dp[i][j] = max(diag, left, top);
        }
    }
    // for (int i = 0; i < sz2; ++i)
    // {
    //     for (int j = 0; j < sz1; ++j)
    //     {
    //         std::cout << dp[i][j] << ' ';
    //     }
    //     std::cout << '\n';
    // }
    return dp.back().back();
}

int main()
{
    std::vector<int> v1 = {6, 3, 1, 3, 2, 3, 2, 1};
    std::vector<int> v2 = {1, 3, 6, 6, 1, 4, 3, 3, 2, 1};
    std::cout << get_LCS_size(v1, v2) << '\n';
}
